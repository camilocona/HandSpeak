package com.example.handspeak

import android.Manifest
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.runtime.*
import androidx.core.content.ContextCompat
import com.example.handspeak.bluetooth.BluetoothController
import com.example.handspeak.ui.components.HomeScreen
import com.example.handspeak.ui.components.LoadingScreen
import com.example.handspeak.ui.theme.HandSpeakTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()

        setContent {
            HandSpeakTheme {
                var showSplash by remember { mutableStateOf(true) }
                val context = this

                var bluetoothController by remember {
                    mutableStateOf<BluetoothController?>(null)
                }

                val receivedLetter = remember { mutableStateOf("?") }

                // Lanzador de permiso para Android 12+
                val bluetoothPermissionLauncher = rememberLauncherForActivityResult(
                    contract = ActivityResultContracts.RequestPermission()
                ) { isGranted ->
                    if (!isGranted) {
                        Toast.makeText(context, "Permiso de Bluetooth denegado", Toast.LENGTH_SHORT).show()
                    } else {
                        // Si el permiso fue concedido, creamos el BluetoothController
                        bluetoothController = BluetoothController(context) { received ->
                            receivedLetter.value = received
                        }
                    }
                }

                if (showSplash) {
                    LoadingScreen(onFinished = {
                        showSplash = false

                        // Verifica permiso antes de crear el controlador
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                            val permission = Manifest.permission.BLUETOOTH_CONNECT
                            if (ContextCompat.checkSelfPermission(context, permission) != PackageManager.PERMISSION_GRANTED) {
                                bluetoothPermissionLauncher.launch(permission)
                            } else {
                                bluetoothController = BluetoothController(context) { received ->
                                    receivedLetter.value = received
                                }
                            }
                        } else {
                            bluetoothController = BluetoothController(context) { received ->
                                receivedLetter.value = received
                            }
                        }
                    })
                } else {
                    HomeScreen(
                        bluetoothController = bluetoothController,
                        receivedLetter = receivedLetter.value,
                        requestPermission = {
                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                                val permission = Manifest.permission.BLUETOOTH_CONNECT
                                if (ContextCompat.checkSelfPermission(context, permission) != PackageManager.PERMISSION_GRANTED) {
                                    bluetoothPermissionLauncher.launch(permission)
                                }
                            }
                        }
                    )
                }
            }
        }
    }
}
