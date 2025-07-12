package com.example.handspeak.ui.components

import androidx.compose.foundation.layout.*
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Shadow
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.*
import androidx.compose.ui.draw.drawWithContent
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.text.font.FontFamily

@Composable
fun NeonTitle(
    modifier: Modifier = Modifier
) {
    Box(
        modifier = modifier.fillMaxWidth(),
        contentAlignment = Alignment.Center
    ) {
        Text(
            text = "Hand Speak",
            textAlign = TextAlign.Center,
            style = TextStyle(
                fontSize = 36.sp,
                fontWeight = FontWeight.ExtraBold,
                fontFamily = FontFamily.SansSerif,
                brush = Brush.linearGradient(
                    listOf(Color.Cyan, Color(0xFF00BFFF)) // Celeste neón
                ),
                shadow = Shadow(
                    color = Color(0xFF00BFFF),
                    offset = Offset(2f, 2f),
                    blurRadius = 12f
                )
            ),
            modifier = Modifier
                .padding(top = 24.dp)
                .drawWithContent {
                    drawContent()
                    drawRect(
                        color = Color(0x3300BFFF),
                        size = size
                    )
                }
        )
    }
}
