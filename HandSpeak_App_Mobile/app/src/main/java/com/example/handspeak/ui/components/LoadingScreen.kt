package com.example.handspeak.ui.components

import androidx.compose.animation.core.*
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.*
import androidx.compose.material3.Surface
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.delay
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.sp
import androidx.compose.material3.Text
import androidx.compose.ui.text.font.FontWeight
import com.example.handspeak.R

@Composable
fun LoadingScreen(onFinished: () -> Unit) {
    var visible by remember { mutableStateOf(true) }

    val alphaAnim by animateFloatAsState(
        targetValue = if (visible) 1f else 0f,
        animationSpec = tween(durationMillis = 2000),
        label = "Fade"
    )

    // Delay para mostrar splash y luego pasar al home
    LaunchedEffect(Unit) {
        delay(2500)
        visible = false
        delay(500)
        onFinished()
    }

    Surface(
        modifier = Modifier.fillMaxSize(),
        color = Color.Black
    ) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(32.dp),
            verticalArrangement = Arrangement.Center,
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            // Puedes cambiar esta imagen por un logo tuyo si lo tienes
            Image(
                painter = painterResource(id = R.drawable.wave),
                contentDescription = "Logo",
                modifier = Modifier
                    .size(120.dp)
                    .alpha(alphaAnim)
            )
            Spacer(modifier = Modifier.height(20.dp))
            Text(
                text = "HandSpeak",
                fontSize = 30.sp,
                color = Color.Cyan,
                fontWeight = FontWeight.Bold,
                modifier = Modifier.alpha(alphaAnim)
            )
        }
    }
}