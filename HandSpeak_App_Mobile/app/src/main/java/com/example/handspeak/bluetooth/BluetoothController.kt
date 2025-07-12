package com.example.handspeak.bluetooth

import android.Manifest
import android.bluetooth.*
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.util.Log
import androidx.core.content.ContextCompat
import kotlinx.coroutines.*
import java.io.IOException
import java.io.InputStream
import java.util.*

class BluetoothController(
    private val context: Context,
    private val onLetterReceived: (String) -> Unit
) {
    private val bluetoothAdapter: BluetoothAdapter? = BluetoothAdapter.getDefaultAdapter()
    private var socket: BluetoothSocket? = null
    private var isReading = false
    private var readJob: Job? = null

    private var SPP_UUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

    private fun hasBluetoothPermission(): Boolean {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.BLUETOOTH_CONNECT
            ) == PackageManager.PERMISSION_GRANTED
        } else true
    }

    fun isBluetoothSupported(): Boolean = bluetoothAdapter != null

    fun isBluetoothEnabled(): Boolean = bluetoothAdapter?.isEnabled == true

    fun getPairedDevices(): Set<BluetoothDevice> {
        if (!hasBluetoothPermission()) {
            Log.w("BluetoothController", "Permiso BLUETOOTH_CONNECT no concedido")
            return emptySet()
        }

        return try {
            bluetoothAdapter?.bondedDevices ?: emptySet()
        } catch (e: SecurityException) {
            Log.e("BluetoothController", "SecurityException en getPairedDevices(): ${e.message}")
            emptySet()
        }
    }

    fun connectToDevice(
        device: BluetoothDevice,
        onConnected: () -> Unit = {},
        onError: (String) -> Unit = {}
    ) {
        if (!hasBluetoothPermission()) {
            onError("Permiso BLUETOOTH_CONNECT no concedido.")
            return
        }

        CoroutineScope(Dispatchers.IO).launch {
            try {
                try {
                    bluetoothAdapter?.cancelDiscovery()
                } catch (e: SecurityException) {
                    Log.e("BluetoothController", "SecurityException cancelando discovery: ${e.message}")
                }

                val tmpSocket: BluetoothSocket? = try {
                    device.createRfcommSocketToServiceRecord(SPP_UUID)
                } catch (e: SecurityException) {
                    Log.e("BluetoothController", "SecurityException creando socket: ${e.message}")
                    withContext(Dispatchers.Main) { onError("Permiso denegado al crear socket") }
                    return@launch
                }

                socket = tmpSocket

                try {
                    socket?.connect()
                } catch (connectException: IOException) {
                    Log.w("BluetoothController", "Conexión estándar fallida, intentando fallback")

                    // fallback con reflexión (uso no oficial)
                    try {
                        val fallbackSocket = device.javaClass
                            .getMethod("createRfcommSocket", Int::class.java)
                            .invoke(device, 1) as BluetoothSocket
                        fallbackSocket.connect()
                        socket = fallbackSocket
                    } catch (fallbackException: Exception) {
                        withContext(Dispatchers.Main) {
                            onError("Error al conectar: ${fallbackException.message}")
                        }
                        Log.e("BluetoothController", "Fallback fallido", fallbackException)
                        return@launch
                    }
                }

                withContext(Dispatchers.Main) {
                    onConnected()
                    startReading()
                }

            } catch (e: SecurityException) {
                withContext(Dispatchers.Main) {
                    onError("Permiso de Bluetooth denegado: ${e.message}")
                }
                Log.e("BluetoothController", "SecurityException general", e)
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    onError("Error al conectar: ${e.message}")
                }
                Log.e("BluetoothController", "Error inesperado", e)
            }
        }
    }

    private fun startReading() {
        val inputStream: InputStream? = try {
            socket?.inputStream
        } catch (e: SecurityException) {
            Log.e("BluetoothController", "SecurityException al obtener InputStream: ${e.message}")
            null
        }

        if (inputStream == null) {
            Log.e("BluetoothController", "InputStream es null o sin permisos")
            return
        }

        isReading = true
        readJob = CoroutineScope(Dispatchers.IO).launch {
            val buffer = ByteArray(1)
            while (isReading) {
                try {
                    val bytes = inputStream.read(buffer)
                    if (bytes > 0) {
                        val receivedChar = buffer[0].toInt().toChar().toString()
                        withContext(Dispatchers.Main) {
                            if (receivedChar[0] in 'A'..'Z') {
                                onLetterReceived(receivedChar)
                            } else {
                                onLetterReceived("?")
                            }
                        }
                    }
                } catch (e: IOException) {
                    Log.e("BluetoothController", "IOException leyendo", e)
                    stopReading()
                } catch (e: SecurityException) {
                    Log.e("BluetoothController", "SecurityException leyendo", e)
                    stopReading()
                }
            }
        }
    }

    private fun stopReading() {
        isReading = false
        readJob?.cancel()
        try {
            socket?.close()
        } catch (e: IOException) {
            Log.e("BluetoothController", "IOException cerrando socket", e)
        } catch (e: SecurityException) {
            Log.e("BluetoothController", "SecurityException cerrando socket", e)
        }
    }
}
