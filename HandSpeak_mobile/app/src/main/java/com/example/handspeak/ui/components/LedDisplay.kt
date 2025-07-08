package com.example.handspeak.ui.components

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.MaterialTheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.*
import androidx.compose.ui.graphics.*
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.*
import androidx.compose.material3.Text
import androidx.compose.ui.draw.shadow
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.text.font.FontStyle

@Composable
fun LedDisplay(letter: String) {
    val displayColor = Color(0xFF00E5FF) // azul celeste tipo neón
    val backgroundColor = Color(0xFF101418) // fondo oscuro sutil
    val glow = displayColor.copy(alpha = 0.6f)

    Box(
        modifier = Modifier
            .fillMaxWidth()
            .height(200.dp)
            .padding(24.dp)
            .background(
                brush = Brush.verticalGradient(
                    colors = listOf(
                        backgroundColor,
                        backgroundColor.copy(alpha = 0.95f)
                    )
                ),
                shape = RoundedCornerShape(16.dp)
            )
            .shadow(10.dp, shape = RoundedCornerShape(16.dp), clip = false)
    ) {
        Text(
            text = letter.ifEmpty { "?" },
            modifier = Modifier
                .align(Alignment.Center)
                .padding(16.dp),
            fontSize = 96.sp,
            fontWeight = FontWeight.ExtraBold,
            color = displayColor,
            textAlign = TextAlign.Center,
            style = MaterialTheme.typography.displayLarge.copy(
                shadow = Shadow(
                    color = glow,
                    offset = Offset(0f, 0f),
                    blurRadius = 24f
                ),
                fontStyle = FontStyle.Normal
            )
        )
    }
}
