package com.example.handspeak.ui.components

import androidx.compose.animation.animateColorAsState
import androidx.compose.animation.core.tween
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*

import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.*
import androidx.compose.ui.draw.shadow
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.*
import kotlinx.coroutines.*

@Composable
fun ConnectButton(
    modifier: Modifier = Modifier,
    onClick: () -> Unit
) {
    var pressed by remember { mutableStateOf(false) }
    val coroutineScope = rememberCoroutineScope()

    val backgroundColor by animateColorAsState(
        targetValue = if (pressed) Color(0xFF00BFFF) else Color(0xFF1E90FF),
        animationSpec = tween(200),
        label = "ColorAnimation"
    )

    Box(
        modifier = modifier
            .height(60.dp)
            .width(200.dp)
            .shadow(elevation = 12.dp, shape = RoundedCornerShape(16.dp))
            .background(
                brush = Brush.verticalGradient(
                    listOf(
                        backgroundColor,
                        backgroundColor.copy(alpha = 0.8f)
                    )
                ),
                shape = RoundedCornerShape(16.dp)
            )
            .clickable(
                onClick = {
                    pressed = true
                    onClick()
                    coroutineScope.launch {
                        delay(150)
                        pressed = false
                    }
                }
            ),
        contentAlignment = Alignment.Center
    )
 {
        Text(
            text = "Conectar",
            color = Color.White,
            fontSize = 20.sp,
            fontWeight = FontWeight.Bold
        )
    }
}
