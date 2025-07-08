package com.example.handspeak.ui.components

import android.Manifest
import android.bluetooth.BluetoothDevice
import android.content.pm.PackageManager
import android.os.Build
import android.widget.Toast
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.*
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Shadow
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.*
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.*
import androidx.core.content.ContextCompat
import com.example.handspeak.bluetooth.BluetoothController

@Composable
fun HomeScreen(
    bluetoothController: BluetoothController?,
    receivedLetter: String,
    requestPermission: () -> Unit
) {
    val context = LocalContext.current

    var showDialog by remember { mutableStateOf(false) }
    var devices by remember { mutableStateOf<List<BluetoothDevice>>(emptyList()) }

    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(Color(0xFF121212))
            .padding(16.dp)
    ) {
        Column(
            modifier = Modifier.fillMaxSize(),
            verticalArrangement = Arrangement.SpaceBetween
        ) {
            // Título con efecto neon
            Text(
                text = "Hand Speak",
                style = TextStyle(
                    fontSize = 60.sp,
                    fontWeight = FontWeight.Bold,
                    color = Color(0xFF00E5FF),
                    shadow = Shadow(
                        color = Color(0xFF0ABFFF),
                        offset = Offset(0f, 0f),
                        blurRadius = 24f
                    ),
                    textAlign = TextAlign.Center
                ),
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(top = 32.dp)
            )

            // Pantalla LED
            LedDisplay(letter = receivedLetter)

            // Botón de conexión
            ConnectButton(
                modifier = Modifier.align(Alignment.CenterHorizontally)
            ) {
                if (bluetoothController?.isBluetoothEnabled() == true) {
                    val hasPermission =
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                            ContextCompat.checkSelfPermission(
                                context,
                                Manifest.permission.BLUETOOTH_CONNECT
                            ) == PackageManager.PERMISSION_GRANTED
                        } else true

                    if (!hasPermission) {
                        requestPermission()
                        return@ConnectButton
                    }

                    try {
                        devices = bluetoothController.getPairedDevices().toList()
                        showDialog = true
                    } catch (e: SecurityException) {
                        Toast.makeText(
                            context,
                            "Permiso de Bluetooth denegado",
                            Toast.LENGTH_SHORT
                        ).show()
                    }
                } else {
                    Toast.makeText(
                        context,
                        "Activa el Bluetooth para continuar",
                        Toast.LENGTH_SHORT
                    ).show()
                }
            }

            Spacer(modifier = Modifier.height(16.dp))

        }

        // Diálogo de selección de dispositivo
        if (showDialog && bluetoothController != null) {
            BluetoothDeviceDialog(
                devices = devices,
                onDismiss = { showDialog = false },
                onDeviceSelected = { device ->
                    bluetoothController.connectToDevice(
                        device,
                        onConnected = {
                            Toast.makeText(
                                context,
                                "Conectado a ${device.name ?: device.address}",
                                Toast.LENGTH_SHORT
                            ).show()
                        },
                        onError = { error ->
                            Toast.makeText(context, error, Toast.LENGTH_LONG).show()
                        }
                    )
                }
            )
        }
    }
}
