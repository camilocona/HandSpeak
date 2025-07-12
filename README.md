# HandSpeak
## Introducción

Actualmente, la inclusión y la accesibilidad son pilares fundamentales en el desarrollo de tecnologías que buscan mejorar la calidad de vida de las personas. Uno de los retos más significativos en este ámbito es la comunicación con personas con discapacidad auditiva que utilizan el lenguaje de señas como principal medio de expresión. El proyecto HandSpeak surge como una solución tecnológica que busca facilitar la interpretación de señas, permitiendo una comunicación más fluida e inclusiva entre personas sordas y oyentes.

HandSpeak consiste en el diseño e implementación de un guante inteligente capaz de reconocer letras del alfabeto en lengua de señas mediante sensores flex en los dedos y un módulo inercial (IMU) que detecta los movimientos de la mano. La información capturada es procesada por una Raspberry Pi Pico y transmitida vía Bluetooth a una aplicación móvil que interpreta y reproduce la letra detectada.

Este sistema busca no solo mejorar la interacción cotidiana de las personas sordas, sino también aportar a la educación, la accesibilidad digital y la construcción de entornos más inclusivos, consolidándose como una herramienta de apoyo en la eliminación de barreras comunicativas.

---
## Procedimiento
Se comenzo con la verificacion de los sensores flex y su debido funcionamiento.

<p align="center">
  <img src="Fotos_proyecto/medicion_sensores.jpg" alt="sensore_flex" width="400"/>
  <br>
  <em>Figura 1. Verificación de sensores flex</em>
</p>

---

Se realizo la proteccion de los sensores flex para evitar daños en estos a la hora de acoplarlos en el guante.


![sensores](Fotos_proyecto/protector_sensores.jpg)

---

se inicio con la creacion del guante, comenzado con el debido aclope de los sensores flex a un guante.

![Descripción visual del tema](Fotos_proyecto/creación_guante.jpg)

Con este guante se presentaron los siguientes problemas:
- Guante demasiado rigido
- Al guante ser muy rigido cuando se intentaba flexionar los sensores se despegaban.
- A la hora de ponerse el guante era muy ancho para las manos de cualquiera de nosotros.
---

Se inicio la caracterizacion de los sensores a traves de divisores de voltaje y circuitos integrados en forma de comparadores(LM324 y LM358)

![Gráfica de resultados](Fotos_proyecto/primer_montaje.jpg)
![Imagen de cierre](Fotos_proyecto/cal_3.3.jpg)
Este circuito se alimento con 3.3V sacados directamente de la Raspberry pi pico W, se establece un umbral de 2.2V para comparar con respecto a los 3.3V de la alimentacion.
Con este montaje se presentaron estos problemas:

- A la salida del comparador nos entregaba un voltaje de 2V o menor a 2V esto provocaba que las entradas GPIO de nuestro MCU no detectara correctamente la entrada como un alto o un bajo(se podia confundir por ruido).
- Al implementar el trimmer para el umbral, este a veces no era muy exacto lo cual generaba errores en la comparacion.

---
#### UPDATES
---

- Para corregir los anteriores problemas se opto por dejar el divisor de voltaje del umbral de comparacion fijo(sin trimmer) en 3.3V.
- Se cambio la alimentacion del circuito a 5V,haciendo una recalibracion de los divisores de voltaje de cada sensores flex.
![Imagen de cierre](Fotos_proyecto/proto_5V.jpg)
![Imagen de cierre](Fotos_proyecto/cal_5V.jpg)

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
