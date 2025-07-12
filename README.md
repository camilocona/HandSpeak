# HandSpeak
##Introducción

---

Se comenzo con la verificacion de los sensores flex y su debido funcionamiento.

<p align="center">
  <img src="Fotos_proyecto/medición_sensores.jpg" alt="sensore_flex" width="400"/>
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

El siguiente paso fue la implementacion del bluetooth como dispositivo de comunicación y de la creacion de la aplicación para mostrar las letras en lenguaje de señas a traves del celular.
![nuevo bluetooth](Fotos_proyecto/Circuito_completo.jpg)

