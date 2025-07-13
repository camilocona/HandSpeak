# HandSpeak
## Introducción

Actualmente, la inclusión y la accesibilidad son pilares fundamentales en el desarrollo de tecnologías que buscan mejorar la calidad de vida de las personas. Uno de los retos más significativos en este ámbito es la comunicación con personas con discapacidad auditiva que utilizan el lenguaje de señas como principal medio de expresión. El proyecto HandSpeak surge como una solución tecnológica que busca facilitar la interpretación de señas, permitiendo una comunicación más fluida e inclusiva entre personas sordas y oyentes.

HandSpeak consiste en el diseño e implementación de un guante inteligente capaz de reconocer letras del alfabeto en lengua de señas mediante sensores flex en los dedos y un módulo inercial (IMU) que detecta los movimientos de la mano. La información capturada es procesada por una Raspberry Pi Pico y transmitida vía Bluetooth a una aplicación móvil que interpreta y reproduce la letra detectada.

Este sistema busca no solo mejorar la interacción cotidiana de las personas sordas, sino también aportar a la educación, la accesibilidad digital y la construcción de entornos más inclusivos, consolidándose como una herramienta de apoyo en la eliminación de barreras comunicativas.

---
## Diseño del sistema de detección de flexión de dedos
Como idea inicial para la detección de señas, se planteó el uso de sensores flex G2 de 2.2 pulgadas, uno por cada dedo. El objetivo era establecer un valor estándar de resistencia para cada sensor, permitiendo una interpretación uniforme de las posiciones de los dedos. Sin embargo, durante las pruebas iniciales se observó que los sensores no presentaban valores de resistencia iguales en condiciones similares, lo que evidenció la necesidad de una calibración individual.

Por ejemplo, el primer sensor presentaba una resistencia de 33 kΩ cuando estaba estirado y de 37.5 kΩ cuando se encontraba doblado. En cambio, el segundo sensor registraba 44 kΩ estirado y 68 kΩ doblado. Esta variabilidad entre sensores obligó a implementar una etapa de acondicionamiento de señal personalizada para cada uno, basada en un divisor de voltaje alimentado a 3.3 V. La resistencia fija del divisor (R₁) se seleccionó como el valor medio entre las resistencias mínima y máxima medidas para cada sensor, permitiendo así generar una tensión de referencia representativa del estado de cada dedo.

La salida del divisor de voltaje fue conectada a un comparador implementado con amplificadores operacionales. Se utilizaron un LM324 (con cuatro amplificadores) y un LM358 (utilizando uno de sus dos amplificadores), lo que permitió procesar las señales de los cinco sensores. Cada comparador recibió también una tensión de referencia de 3.3 V, generada mediante otro divisor de voltaje, ya que la alimentación general del sistema era de 5 V. De este modo, cuando el sensor flex estaba estirado, la tensión de salida del divisor era mayor que 3.3 V, provocando que el comparador generara una salida de aproximadamente el 70 % de la tensión de alimentación (≈3.6 V). En cambio, cuando el dedo se doblaba, la resistencia del sensor aumentaba, reduciendo la tensión en el divisor y provocando que el comparador entregara una salida cercana a 0 V (nivel bajo o tierra).

Inicialmente, se intentó conectar directamente las salidas de los comparadores a los pines GPIO de la Raspberry Pi Pico (RP2040); sin embargo, esto ocasionaba lecturas erróneas de las letras detectadas por el guante. Tras revisar la documentación técnica de la RP2040, se identificó que el voltaje máximo admitido en sus entradas GPIO es de 3.3 V. Por lo tanto, fue necesario incorporar un segundo divisor de voltaje en cada línea de salida antes de ingresar a la RP2040, asegurando así la integridad de las señales digitales y el correcto funcionamiento del sistema de lectura. A contnuación se presentan fotografías evidenciando el proceso descrito.

<p align="center">
  <img src="Fotos_proyecto/circuito.png" alt="sensore_flex" width="800"/>
  <br>
  <em>Figura 1. Esquema del sistema de comparación de voltaje utilizando un amplificador operacional LM324. El voltaje de entrada varía según la resistencia del sensor flex (dependiente de la posición del dedo) y se compara con una referencia de 3.3 V. La salida del comparador pasa por un divisor de voltaje adicional para adecuarse al nivel máximo permitido por las entradas GPIO de la Raspberry Pi Pico (3.3 V).</em>
</p>

---
## Procedimiento de montaje del guante
Se comenzó con la verificacion de los sensores flex y su debido funcionamiento.

<p align="center">
  <img src="Fotos_proyecto/medicion_sensores.jpg" alt="sensore_flex" width="400"/>
  <br>
  <em>Figura 2. Verificación de sensores flex.</em>
</p>


Se realizó la proteccion de los sensores flex para evitar daños en estos a la hora de acoplarlos en el guante.
<p align="center">
  <img src="Fotos_proyecto/protector_sensores.jpg" alt="sensore_flex" width="400"/>
  <br>
  <em>Figura 3. Protección de los sensores con termoencogible.</em>
</p>


Se inició con la creacion del guante, comenzado con el debido aclope de los sensores flex a un guante.
<p align="center">
  <img src="Fotos_proyecto/creación_guante.jpg" alt="sensore_flex" width="400"/>
  <br>
  <em>Figura 4. Protección de los sensores con termoencogible.</em>
</p>

Con este guante se presentaron los siguientes problemas:
- Guante demasiado rigido
- Al guante ser muy rigido cuando se intentaba flexionar los sensores se despegaban.
- A la hora de ponerse el guante era muy ancho para las manos de cualquiera de nosotros.
---

Para interpretar correctamente las variaciones de resistencia en los sensores flex instalados en los dedos del guante, se implementó una etapa de caracterización basada en comparadores de voltaje utilizando amplificadores operacionales LM324 y LM358.
<p align="center">
  <img src="Fotos_proyecto/primer_montaje.jpg" alt="sensore_flex" width="400"/>
  <br>
  <em>Figura 5. Caracterización de los sensores mediante amplificadores operacionales comparadores (LM324 y LM358).</em>
</p>

Inicialmente se realizaron los siguientes cálculos:
<p align="center">
  <img src="Fotos_proyecto/cal_3.3.jpg" alt="sensore_flex" width="400"/>
  <br>
  <em>Figura 6. Primeros cálculos realizados.</em>
</p>

Este circuito fue alimentado con un voltaje de 3.3 V proveniente directamente de la Raspberry Pi Pico W. Se estableció un umbral de referencia de 2.2 V para realizar la comparación con respecto a los 3.3 V de la alimentación.

Durante la implementación de este montaje se presentaron los siguientes inconvenientes:
- La salida del comparador entregaba un voltaje igual o inferior a 2 V, lo que provocaba que las entradas GPIO del microcontrolador no reconocieran correctamente los niveles lógicos. Esto generaba ambigüedad en la detección del estado alto o bajo, especialmente en presencia de ruido eléctrico.
- Al utilizar un trimmer para ajustar el umbral de comparación, se observó una falta de precisión en ciertos casos, lo cual derivaba en errores en el proceso de comparación.

---


## Resistencias de los sensores flex (Cálculos finales para 5V)
- Para corregir los problemas mencionados previamente, se optó por fijar el divisor de voltaje correspondiente al umbral de comparación en 3.3 V, eliminando el uso del trimmer para evitar errores de ajuste.

- Además, se cambió la alimentación del circuito a 5 V, lo que requirió una recalibración de los divisores de voltaje asociados a cada sensor flex, a fin de garantizar lecturas correctas y compatibles con el nuevo nivel de referencia.
### Pulgar (Se realizó esto de igual forma para cada uno de los demás dedos)

- **Extendido:** 33 kΩ  
- **Flexionado:** 37.5 kΩ

Se seleccionó una resistencia media para el divisor de voltaje:

$$
R_1 = 35\,k\Omega
$$

Para calcular \( R_2 \), se utilizó la fórmula del divisor de voltaje:

$$
R_2 = \frac{V_\text{out} \cdot R_1}{V_\text{in} - V_\text{out}}
$$

Sustituyendo los valores:

$$
R_2 = \frac{2.2 \cdot 35\,k\Omega}{3.3 - 2.2}
= \frac{77\,k\Omega}{1.1} = 70\,k\Omega
$$

**Resultado:**

$$
\boxed{R_2 = 70\,k\Omega}
$$

---

Del mismo modo, se calcularon los valores para los demás dedos, obteniendo:

### Índice
$$
\boxed{R_2 = 103\,k\Omega}
$$

### Corazón
$$
\boxed{R_2 = 128\,k\Omega}
$$

### Anular
$$
\boxed{R_2 = 71.4\,k\Omega}
$$

### Meñique
$$
\boxed{R_2 = 91.2\,k\Omega}
$$

### Divisor de voltaje para el comparador (umbral de 3.3 V)
$$
\boxed{R_2 = 99\,k\Omega}
$$

### Divisor de voltaje para la entrada GPIO (máximo 3.3 V)
$$
\boxed{R_2 = 22\,k\Omega}
$$

---
#### Mejoras en el diseño físico
---



- Se realizó la actualización a un nuevo guante con un diseño más ergonómico, lo cual permitió una mejor sujeción de los sensores flex y, en consecuencia, una lectura más precisa y estable de sus valores de resistencia.

<p align="center">
  <img src="Fotos_proyecto/Guante_2.jpg" alt="Guante_2" width="400"/>
  <br>
  <em>Figura 9. Guante versión 2.0.</em>
</p>


- Se adaptó el nuevo guante al circuito en protoboard, sin embargo, se identificó un problema relacionado con el diseño físico de la protoboard: las pistas internas eran demasiado anchas, lo que provocaba un contacto deficiente con las resistencias. Esta situación generaba lecturas erróneas en los divisores de voltaje, afectando la precisión del sistema de detección.


<p align="center">
  <img src="Fotos_proyecto/Montaje_Circuito_Guante_Sin_IMU.jpg" alt="Circuit_Sin_IMU" width="400"/>
  <br>
  <em>Figura 10. Montaje del circuito sin IMU.</em>
</p>


- Se optó finalmente por realizar el montaje del circuito sobre una baquela universal, con el objetivo de evitar errores de contacto frecuentes en la protoboard y lograr un diseño más compacto, robusto y confiable.

- Se incorporó una IMU GY-91 al guante, permitiendo iniciar las pruebas con reconocimiento de movimiento. Esta adición fue muy importante para diferenciar letras que comparten el mismo patrón de posición de los dedos pero difieren en su dinámica. Por ejemplo, las letras "I" y "J" presentan configuraciones similares en cuanto a dedos alzados y retraídos; sin embargo, gracias a la IMU fue posible distinguir la letra "J" mediante la detección de movimiento, mejorando así la precisión del reconocimiento.



<p align="center">
  <img src="Fotos_proyecto/PCB.jpg" alt="PCB" width="400"/>
  <br>
  <em>Figura 11. Montaje en baquela universal.</em>
</p>

El siguiente paso en el desarrollo fue la implementación de un módulo Bluetooth como medio de comunicación entre el guante y un dispositivo externo. Paralelamente, se desarrolló una aplicación móvil encargada de recibir y visualizar las letras detectadas por el sistema. Esta aplicación permite interpretar las señas realizadas por el usuario y mostrarlas en la pantalla del celular,

<p align="center">
  <img src="Fotos_proyecto/Circuito_completo.jpg" alt="Circuito_completo" width="400"/>
  <br>
  <em>Figura 12. Circuito con bluetooth.</em>
</p>

<p align="center">
  <img src="Fotos_proyecto/Aplicacion.jpg" alt="Aplicacion" width="400"/>
  <br>
  <em>Figura 13. Aplicación creada.</em>
</p>

Inicialmente se utilizó un módulo Bluetooth adquirido bajo la referencia HC-05. Sin embargo, durante las pruebas se detectaron inconsistencias en su funcionamiento que sugerían que el módulo no correspondía exactamente a dicha referencia, a pesar de haber sido adquirido como tal. Esta situación generó dificultades en el proceso de comunicación, lo que motivó una verificación más detallada.


<p align="center">
  <img src="Fotos_proyecto/Bluetooth_HC-08.jpg" alt="Bluetooth_HC-08" width="400"/>
  <br>
  <em>Figura 14. Bluetooth con la referencia mala.</em>
</p>

Con la implementación de este módulo Bluetooth y la creación de la aplicación se presentaron los siguientes problemas:


<p align="center">
  <img src="Fotos_proyecto/Error_bluetooth.jpg" alt="Error_bluetooth" width="400"/>
  <br>
  <em>Figura 15. Error_bluetooth.</em>
</p>

- La aplicacion no establecia una comunicacion con el modulo bluetooth. Entregaba un error de socket, lo que quiere decir que no se establecia una comunicacion entre el celular y el modulo bluetooth.
 
Para poder saber cual era el problema fue necesario descargar la aplicacion llamada Serial Bluetooth Terminal que se encuentra en la Play Store de google. Esto con el fin de descartar si el modulo que se instalo si estaba transmitiendo los datos que se querian enviar a la aplicacion movil o si era la aplicacion movil la que tenia el error.

 <p align="center">
  <img src="Fotos_proyecto/Serial_Bluetooth_Terminal.jpg" alt="SBT" width="400"/>
  <br>
  <em>Figura 16. Aplicacion Serial Bluetooth Terminal (SBT).</em>
</p>

CONTEXTO SOBRE LA APLICACION SERIAL BLUETOOTH TERMINAL:
 Esta aplicacion permite conectar y mostrar lo que se esta enviando desde la raspberry pi pico en este caso o enviar informacion a traves de lpierto serial de ela aplicacion por medio de un modulo bluetooth.
 La aplicacion tiene la caracteristica de dejar conectar tanto modulos BLE y Modulos de bluetooth clasicos.

<p align="center">
  <img src="Fotos_proyecto/SBT_bluetooth_classic.jpg" alt="SBT_Classic" width="400"/>
  <br>
  <em>Figura 17. Reconocimiento de dispositivos bluetooth classic.</em>
</p>
 

<p align="center">
  <img src="Fotos_proyecto/SBT_bluetooth_LE.jpg" alt="SBT_BLE" width="400"/>
  <br>
  <em>Figura 18. Reconocimiento de dispositivos bluetooth LE.</em>
</p>

-Ahora sabiendo un poco mas de la aplicacion se procede a verificar que si conectara el bluetooth y se enviara la informacion desde la rasberry pi pico a la aplicacion. 


<p align="center">
  <img src="Fotos_proyecto/SBT_Comunicacion.jpg" alt="SBT_Comunicacion" width="400"/>
  <br>
  <em>Figura 19. Comunicación entre raspberry pi pico y SBT.</em>
</p>


Al realizar esta prueba se pudo constatar que el modulo bluetooth si estaba transmitiendo lo datos, yu el codigo de las rapberry si estaba realizando bien su trabajo. Ya con lo anterior descartado se verifica bien si el porblema lo tenia la aplicacion movil, en esta parte encontramos  que la aplicacion no tenia errores para la configuracion de un modulo de bluetooth HC-05, el cual es un bluetooth clasico segun su hoja de datos. Revisando y analizando bien en la aplicacion Serial Bluetooth Terminal (SBT). se pudo observar que el modulo bluetooth que se compro no era la referencia HC-05, ya que en la aplicacion SBT nos mostraba que se enlazaba como un bluetooth de bajo consumo (Bluetooth Low Energy BLE). 

Al saber lo anterior y teniendo en cuenta que la aplicacion movil que se diseño con los permisos y protocolos para un bluetooth clasico no era posible que se estableciera una conecxion entre el bluetooth y la aplicacion, por esto se generaba el error.

Para  solucionar este problema se cambio el modulo por un modulo que verdaderamente fuera el HC-05, logrando asi que la aplicación se enlazara con la raspberry pi pico.

<p align="center">
  <img src="Fotos_proyecto/Bluetooth_HC-05.jpg" alt="Bluetooth_HC-05" width="400"/>
  <br>
  <em>Figura 20. Modulo bluetooth HC-05.</em>
</p>


Y lograr que se mostrara la informacion esperada por medio de la aplicacion.
![BLuetooth_HC-05](Fotos_proyecto/Aplicacion_funcionando.jpg)

<p align="center">
  <img src="Fotos_proyecto/Aplicacion_funcionando.jpg" alt="Aplicacion_funcionando" width="400"/>
  <br>
  <em>Figura 21. Aplicación en funcionamiento.</em>
</p>

Con la solucion al problema del bluetooth se logro mostrar todas las letras del abecedario en lenguaje de señas por mediod la aplicación.
