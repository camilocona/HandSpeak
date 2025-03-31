# HandSpeak: Guante Traductor de Lenguaje de Señas

## Integrantes
- Camilo Andres Anacona Anacona
- Maria Valentina Quiroga Alzate
- Simón Llano Cárdenas
- Mario Andres Leal Galvis

---



## Introducción
La barrera del desconocimiento de este idioma(señas) limita la inclusión en la sociedad. Este proyecto busca desarrollar un guante electrónico que detecte signos en lenguaje de señas y los convierta en texto o voz, facilitando la comunicación con personas que no dominan esta forma de expresión.

## Objetivo General
Desarrollar un guante traductor de lenguaje de señas basado en la Raspberry Pi Pico, capaz de identificar letras del alfabeto dactilológico mediante sensores flexibles y un acelerómetro, para luego mostrar el resultado en una pantalla LCD.

## Objetivos Específicos
- Implementar un sistema de sensores que detecte la posición de los dedos mediante sensores flexibles.
- Incorporar un MPU6050 para capturar movimientos e inclinaciones de la mano.
- Desarrollar un algoritmo en C que compare los valores con umbrales predefinidos para identificar letras.
- Mostrar el resultado en una pantalla LCD I2C
- Diseñar un guante ergonómico donde los sensores estén integrados de manera funcional.

## Metodología
### Adquisición de Datos
- Los sensores flexibles en cada dedo medirán la curvatura (entrada analógica).
- El MPU6050 proporcionará datos de aceleración y orientación de la mano (I2C).

### Procesamiento de Datos en Raspberry Pi Pico
- Se definirán umbrales de referencia para cada letra del abecedario.
- Un programa en C (con el SDK de Pico) comparará los datos en tiempo real.
- Se asignará una letra en función de las posiciones de los dedos y la inclinación de la mano.

### Salida de Datos
- La letra reconocida se mostrará en una pantalla LCD.
- De igual forma se espera implementar una salida de audio.


## Resultados Esperados
- Creación de un prototipo funcional del guante.
- Precisión aceptable en el reconocimiento de letras mediante sensores predefinidos.
- Comunicación efectiva con pantalla LCD y salida de audio.
