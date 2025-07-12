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


Se comenzó con la verificacion de los sensores flex y su debido funcionamiento.

<p align="center">
  <img src="Fotos_proyecto/medicion_sensores.jpg" alt="sensore_flex" width="400"/>
  <br>
  <em>Figura 1. Verificación de sensores flex.</em>
</p>


Se realizó la proteccion de los sensores flex para evitar daños en estos a la hora de acoplarlos en el guante.
<p align="center">
  <img src="Fotos_proyecto/protector_sensores.jpg" alt="sensore_flex" width="400"/>
  <br>
  <em>Figura 2. Protección de los sensores con termoencogible.</em>
</p>


Se inició con la creacion del guante, comenzado con el debido aclope de los sensores flex a un guante.
<p align="center">
  <img src="Fotos_proyecto/creación_guante.jpg" alt="sensore_flex" width="400"/>
  <br>
  <em>Figura 3. Protección de los sensores con termoencogible.</em>
</p>

Con este guante se presentaron los siguientes problemas:
- Guante demasiado rigido
- Al guante ser muy rigido cuando se intentaba flexionar los sensores se despegaban.
- A la hora de ponerse el guante era muy ancho para las manos de cualquiera de nosotros.
---

Se inicio la caracterizacion de los sensores a traves de divisores de voltaje y circuitos integrados en forma de comparadores(LM324 y LM358)


<p align="center">
  <img src="Fotos_proyecto/primer_montaje.jpg" alt="primer_montaje" width="400"/>
  <br>
  <em>Figura 4. Montaje en protoboard.</em>
</p>


<p align="center">
  <img src="Fotos_proyecto/cal_3.3.jpg" alt="Calculos_33v" width="400"/>
  <br>
  <em>Figura 5. Calculos con 3.3v.</em>
</p>


Este circuito se alimento con 3.3V sacados directamente de la Raspberry pi pico W, se establece un umbral de 2.2V para comparar con respecto a los 3.3V de la alimentacion.
Con este montaje se presentaron estos problemas:

- A la salida del comparador nos entregaba un voltaje de 2V o menor a 2V esto provocaba que las entradas GPIO de nuestro MCU no detectara correctamente la entrada como un alto o un bajo(se podia confundir por ruido).
- Al implementar el trimmer para el umbral, este a veces no era muy exacto lo cual generaba errores en la comparacion.


## Resistencias de los sensores flex

### Pulgar

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
#### UPDATES
---

- Para corregir los anteriores problemas se opto por dejar el divisor de voltaje del umbral de comparacion fijo(sin trimmer) en 3.3V.
- Se cambio la alimentacion del circuito a 5V,haciendo una recalibracion de los divisores de voltaje de cada sensores flex.

<p align="center">
  <img src="Fotos_proyecto/proto_5V.jpg" alt="Calculos_5v" width="400"/>
  <br>
  <em>Figura 6. Calculos con 3.3v.</em>
</p>


<p align="center">
  <img src="Fotos_proyecto/cal_5V.jpg" alt="cal_5V" width="400"/>
  <br>
  <em>Figura 7. Calculos con 5v.</em>
</p>

- Se realizo la actualizacion de un nuevo guanto mas ergonomico el cual nos permitio una mejor lectura de los sensores Flex
![Imagen de cierre](Fotos_proyecto/Guante_2.jpg)

- Se adacto el nuevo guante con el nuevo circuito de protoboard pero se encontro el problema de la protoboard, ya que esta trae las pistas muy anchas y las resistencias quedaban con mal contacto, generando valores erroneos en los divisores.
![nuevo guante](Fotos_proyecto/Montaje_Circuito_Guante_Sin_IMU.jpg)
- Se opto como ultimo circuito un montaje en Baquela universal para evitar errores de contactos y de esta manera tener un circuito mas compacto.
- Se agrego la IMU(GY91) al guante para iniciar las pruebas con movimiento, obteniendo buenos resultados con esta referencia de IMU.
![nuevo pcb](Fotos_proyecto/PCB.jpg)

El siguiente paso fue la implementacion del bluetooth como dispositivo de comunicación y de la creacion de una aplicación movil para mostrar las letras en lenguaje de señas a traves del celular.
![nuevo bluetooth](Fotos_proyecto/Circuito_completo.jpg)
![nuevo bluetooth](Fotos_proyecto/Aplicacion.jpg)

Inicialmente se utilizo un modulo Bluetooth que aparentemente era de referencia HC-05 o con esa referencia fue que se compro.
![bluetooth_HC-08](Fotos_proyecto/Bluetooth_HC-08.jpg)

Con la implementacion de este modulo bluetooth y la creacion de la aplicacion se presentaron los siguientes problemas:

  ![bluetooth_HC-08](Fotos_proyecto/Error_bluetooth.jpg)
- La aplicacion no establecia una comunicacion con el modulo bluetooth. Entregaba un error de socket, lo que quiere decir que no se establecia una comunicacion entre el celular y el modulo bluetooth.
 
Para poder saber cual era el problema fue necesario descargar la aplicacion llamada Serial Bluetooth Terminal que se encuentra en la Play Store de google. Esto con el fin de descartar si el modulo que se instalo si estaba transmitiendo los datos que se querian enviar a la aplicacion movil o si era la aplicacion movil la que tenia el error.
 ![SBT](Fotos_proyecto/Serial_Bluetooth_Terminal.jpg)

CONTEXTO SOBRE LA APLICACION SERIAL BLUETOOTH TERMINAL:
 Esta aplicacion permite conectar y mostrar lo que se esta enviando desde la raspberry pi pico en este caso o enviar informacion a traves de lpierto serial de ela aplicacion por medio de un modulo bluetooth.
 La aplicacion tiene la caracteristica de dejar conectar tanto modulos BLE y Modulos de bluetooth clasicos.
 ![SBT](Fotos_proyecto/SBT_bluetooth_classic.jpg)
 ![SBT](Fotos_proyecto/SBT_bluetooth_LE.jpg)

-Ahora sabiendo un poco mas de la aplicacion se procede a verificar que si conectara el bluetooth y se enviara la informacion desde la rasberry pi pico a la aplicacion. 
![SBT](Fotos_proyecto/SBT_Comunicacion.jpg)

Al realizar esta prueba se pudo constatar que el modulo bluetooth si estaba transmitiendo lo datos, yu el codigo de las rapberry si estaba realizando bien su trabajo. Ya con lo anterior descartado se verifica bien si el porblema lo tenia la aplicacion movil, en esta parte encontramos  que la aplicacion no tenia errores para la configuracion de un modulo de bluetooth HC-05, el cual es un bluetooth clasico segun su hoja de datos. Revisando y analizando bien en la aplicacion Serial Bluetooth Terminal (SBT). se pudo observar que el modulo bluetooth que se compro no era la referencia HC-05, ya que en la aplicacion SBT nos mostraba que se enlazaba como un bluetooth de bajo consumo (Bluetooth Low Energy BLE). 

Al saber lo anterior y teniendo en cuenta que la aplicacion movil que se diseño con los permisos y protocolos para un bluetooth clasico no era posible que se estableciera una conecxion entre el bluetooth y la aplicacion, por esto se generaba el error.

Para  solucionar este problema se cambio el modulo por un modulo que verdaderamente fuera el HC-05, logrando asi que la aplicación se enlazara con la raspberry pi pico.
![BLuetooth_HC-05](Fotos_proyecto/Bluetooth_HC-05.jpg)

Y lograr que se mostrara la informacion esperada por medio de la aplicacion.
![BLuetooth_HC-05](Fotos_proyecto/Aplicacion_funcionando.jpg)

Con la solucion al problema del bluetooth se logro mostrar todas las letras del abecedario en lenguaje de señas por mediod la aplicación.
