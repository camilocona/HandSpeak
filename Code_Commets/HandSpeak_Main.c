#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
//#include "hardware/rtc.h"
#include "hardware/timer.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "hardware/uart.h"

/*Entradas/salidas (gpio)

Comunicación I2C (i2c)

Temporizadores (timer)

Comunicación UART (uart)*/

// ------------------ UART para Bluetooth ------------------
#define UART_ID uart0 //Define que se usará la interfaz UART 0 para enviar/recibir datos (en este caso, al módulo Bluetooth).
#define BAUD_RATE 9600 //Define la velocidad de transmisión serial: 9600 bits por segundo (bps). Es un valor estándar compatible con muchos módulos Bluetooth (como HC-05/HC-06).

#define UART_TX_PIN 16 //Asigna los pines GPIO 16 como TX (transmisión) y GPIO 17 como RX (recepción), que son los valores recomendados para uart0.
#define UART_RX_PIN 17

/*Define el uso de UART0 con velocidad de 9600 bps, 
y los pines GPIO 16 (TX) y 17 (RX) para comunicarse con el módulo Bluetooth.*/

// ------------------ Pines de dedos ------------------
//Asigna cada dedo a un pin GPIO que recibe el estado (flexionado o no) desde los comparadores.
#define PIN_PULGAR 3
#define PIN_INDICE 4
#define PIN_MEDIO 5
#define PIN_ANULAR 7
#define PIN_MENIQUE 8

// ------------------ I2C del sistema de letras (se deja por compatibilidad) ------------------
#define I2C_SDA 20
#define I2C_SCL 21

// ------------------ I2C para GY-91 (MPU9250 en i2c1, GP26/27) ------------------
/*- Define que el bus I2C que se va a usar es el **I2C1** del microcontrolador (la RP2040 tiene dos: `i2c0` e `i2c1`).
- Este es el **bus activo real** que se usa para comunicarte con la IMU MPU9250.*/
#define I2C_PORT i2c1
#define SDA_PIN 26
#define SCL_PIN 27
//Todos los dispositivos conectados al mismo bus (SDA/SCL) deben tener una dirección única, como si fuera una dirección postal.
//Cuando la Raspberry Pi Pico (el maestro) quiere hablar con el MPU9250 (el esclavo), necesita saber a qué dirección enviar comandos o leer datos. Esa dirección es 0x68.
#define MPU9250_ADDR 0x68
#define PWR_MGMT_1 0x6B //Es el registro de gestión de energía 1 (Power Management 1) del MPU9250.
#define ACCEL_XOUT_H 0x3B //se hace para definir una constante simbólica que representa la dirección del registro del sensor MPU9250 
//donde comienza la lectura de datos del acelerómetro, 
//específicamente el byte más significativo (High Byte) del eje X, El MPU9250 entrega los datos del acelerómetro en formato big-endian, es decir:
//El byte más importante (MSB) va primero, en ACCEL_XOUT_H

// ------------------ Configuración IMU ------------------
#define IMU_UMBRAL 0.05f // umbral de aceleración en "g" para detectar movimiento

// ------------------ Bandera para polling IMU ------------------
volatile bool flag_tiempo_imu = false; //Declara una bandera de control que se usa para indicar cuándo es el momento de leer la IMU (MPU9250).

// ------------------ Timer callback (interrupción periódica) ------------------
//Es una rutina de interrupción programada por temporizador que se ejecuta cada 3 segundos
//Esta es importante para activar el procesamiento en el main() sin tener que leer la IMU continuamente.
int64_t repetir_lectura_imu(alarm_id_t id, void *user_data) {
    flag_tiempo_imu = true; //Es hora de leer la IMU (acelerómetro del MPU9250)
    add_alarm_in_ms(3000, repetir_lectura_imu, NULL, false); // reprogramar cada 3s
    return 0;
}

// ------------------ Inicialización MPU9250 ------------------
//"Escribe el valor 0x00 en el registro 0x6B, para salir del modo sleep y comenzar a funcionar."
void mpu9250_init() {
    uint8_t buf[] = {PWR_MGMT_1, 0x00}; //Quitar modo sleep, Crea un arreglo de 2 bytes: 
    //Primer byte: la dirección del registro PWR_MGMT_1 (0x6B) Segundo byte: el valor 0x00, que desactiva el modo de suspensión
    i2c_write_blocking(I2C_PORT, MPU9250_ADDR, buf, 2, false); //False indica que sí se debe enviar una condición de STOP al final de la transmisión I²C. 
    //En I²C, una condición STOP indica que la transmisión ha terminado, liberando el bus.
}

// ------------------ Lectura de acelerómetro ------------------
bool leer_acelerometro(float *ax, float *ay, float *az) { //Devuelve true si la lectura fue exitosa, false si falló. 
    //Recibe tres punteros a float: ax, ay, az, donde se colocarán las aceleraciones en g.
    
    uint8_t reg = ACCEL_XOUT_H; //la dirección del registro donde comienzan los datos del acelerómetro. 
    //Guarda esa dirección en la variable reg para usarla como referencia en la lectura I²C.
    
    uint8_t datos[6]; //Arreglo donde se almacenarán los 6 bytes leídos: 2 bytes para X (MSB + LSB) 2 bytes para Y 2 bytes para Z
    if (i2c_write_blocking(I2C_PORT, MPU9250_ADDR, &reg, 1, true) < 0) return false; 
      /*Envía al sensor el byte 0x3B (inicio de datos de aceleración).
      true indica que no se envía STOP, porque inmediatamente después se hará una lectura.
      Si ocurre un error (retorno < 0), la función termina con false.*/
    if (i2c_read_blocking(I2C_PORT, MPU9250_ADDR, datos, 6, false) < 0) return false;
      /*Lee 6 bytes desde la dirección previamente enviada.
      Los guarda en el arreglo datos.
      false indica que sí se envía STOP al finalizar la transmisión.*/

  //El MPU9250 entrega los valores del acelerómetro como enteros de 16 bits con signo (int16_t) para cada eje (X, Y, Z). En esta parte del código se reconstruyen.
  //Pero por el bus I²C, los datos se envían en partes de 8 bits (1 byte), así que cada eje se transmite en dos bytes consecutivos

  //El MPU9250 entrega los valores de aceleración como números de 16 bits con signo (int16_t).
//Pero como el bus I²C transmite de a 8 bits (1 byte) por vez, el sensor divide ese número de 16 bits en dos bytes consecutivos
    int16_t raw_x = (datos[0] << 8) | datos[1];
    int16_t raw_y = (datos[2] << 8) | datos[3];
    int16_t raw_z = (datos[4] << 8) | datos[5];


  //se hacen para convertir los datos crudos del acelerómetro (raw_x, raw_y, raw_z) a valores de aceleración real en unidades de gravedad (g).

  //el MPU9250, cuando está configurado en el rango ±2g, tiene una resolución de 16 bits, lo que significa que los valores crudos (raw_x, raw_y, raw_z) van desde:
  //-32768 (−2g) a +32767 (+2g)

  //El valor crudo correspondiente a +1g es: 1g = 16384 unidades digitales, por eso se tiene que dividir entre 16384 para obtener el valor real en unidades de gravedad
    *ax = raw_x / 16384.0f;
    *ay = raw_y / 16384.0f;
    *az = raw_z / 16384.0f;
    return true;
}

// ------------------ Detección de letras ------------------
//Función que traduce la posición de los dedos (y el movimiento detectado por la IMU) en una letra del abecedario en lenguaje de señas.
char detectar_letra(uint8_t p, uint8_t i, uint8_t m, uint8_t a, uint8_t me, bool imu_activo) {
    uint8_t cod = (p << 4) | (i << 3) | (m << 2) | (a << 1) | me; //Cada dedo representa un bit

    switch (cod) {
        case 0b10000: return imu_activo ? 'E' : 'A';
        case 0b00000: return imu_activo ? 'O' : 'C';
        case 0b01000: return imu_activo ? 'S' : 'D';
        case 0b10111: return imu_activo ? 'T' : 'F';
        case 0b00001: return imu_activo ? 'J' : 'I';
        case 0b11100: return imu_activo ? 'H' : 'K';
        case 0b11000: return imu_activo ? 'G' : 'L';
        case 0b01110: return imu_activo ? 'M' : 'W';
        case 0b01100: return imu_activo ? 'N' : 'V';
        case 0b10001: return imu_activo ? 'X' : 'Y';
        case 0b11111: return imu_activo ? 'Q' : ' ';  // Espacio
        // Letras únicas sin IMU
        case 0b01011: return 'P';
        case 0b10100: return 'R';
        case 0b11001: return 'U';
        case 0b00111: return 'Z';
        case 0b01111: return 'B';
        default: return '-';
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);  // Solo se deja aquí para permitir que se abra la terminal, Espera 2 segundos para que dé tiempo de abrir la consola

    printf("Inicializando sistema HandSpeak + IMU\n");

    // ------------------ Inicializar GPIO de dedos ------------------
    for (int pin = PIN_PULGAR; pin <= PIN_MENIQUE; pin++) {
        gpio_init(pin); //Inicializa el pin
        gpio_set_dir(pin, GPIO_IN); //Lo configura como entrada
        gpio_pull_down(pin); //Activa una resistencia pull-down interna, para que el pin esté en 0 (LOW) cuando no reciba señal
    }

    // ------------------ Inicializar IMU (i2c1 en GP26 y GP27) ------------------
    i2c_init(I2C_PORT, 400000); //Compatible con el MPU9250: este sensor soporta hasta 400 kHz en I²C
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C); // Configura los pines como líneas I²C (SDA y SCL).
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C); // Configura los pines como líneas I²C (SDA y SCL).
    gpio_pull_up(SDA_PIN); //Habilita resistencias pull-up, necesarias para I²C.
    gpio_pull_up(SCL_PIN); //Habilita resistencias pull-up, necesarias para I²C.
    mpu9250_init();

    // ------------------ Iniciar timer para polling+interrupción IMU ------------------
    /*Programa una interrupción cada 3 s para leer la IMU.La función repetir_lectura_imu activará una bandera (flag_tiempo_imu) que habilita la lectura en el while*/
    add_alarm_in_ms(3000, repetir_lectura_imu, NULL, false);


    // ------------------ Inicializar UART para Bluetooth ------------------
    uart_init(UART_ID, BAUD_RATE); //Inicializa la UART a 9600 baudios.
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); //Configura los pines para transmitir (TX) y recibir (RX) datos vía Bluetooth.
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); //Configura los pines para transmitir (TX) y recibir (RX) datos vía Bluetooth.


    char ultima_letra = '-'; //sirve para almacenar la última letra que fue detectada y enviada, con el propósito de evitar repeticiones innecesarias.

    while (true) {
        // Leer estado de dedos
        uint8_t p = !!gpio_get(PIN_PULGAR); //Lee el estado de cada dedo (HIGH o LOW). !! convierte cualquier valor distinto de cero a 1 (fuerza que sea 0 o 1).
        uint8_t i = !!gpio_get(PIN_INDICE);
        uint8_t m = !!gpio_get(PIN_MEDIO);
        uint8_t a = !!gpio_get(PIN_ANULAR);
        uint8_t me = !!gpio_get(PIN_MENIQUE);

        // Solo leer IMU y detectar letra si la bandera se activó
        //En cada iteración del bucle principal, el código "pregunta" constantemente (polling) si la bandera flag_tiempo_imu se activó.
        if (flag_tiempo_imu) {
            flag_tiempo_imu = false;

            float ax = 0, ay = 0, az = 0;
            bool imu_mov = false; //por defecto se asume que el guante está quieto.

            if (leer_acelerometro(&ax, &ay, &az)) {
                float magnitud = sqrtf(ax * ax + ay * ay + az * az); //Se calcula la magnitud total del vector de aceleración.
                //si el dispositivo está quieto, la magnitud del vector debe ser aproximadamente 1.0g (solo siente la gravedad). Si se está moviendo, esta magnitud cambia.
                float delta = fabsf(magnitud - 1.0f); //Calcula la diferencia (en valor absoluto) entre la magnitud total calculada y el valor esperado en reposo (1g).
                imu_mov = delta > IMU_UMBRAL; //Evalúa si el cambio detectado en la aceleración (delta) es suficiente para considerarse movimiento.

                printf("Magnitud: %.3f | Delta: %.3f | IMU: %s\n", magnitud, delta, imu_mov ? "MOV" : "QUIETO");
            }

            char letra = detectar_letra(p, i, m, a, me, imu_mov); //se utiliza para traducir la posición de los dedos y el movimiento del guante (detectado con la IMU) en una letra del abecedario en lenguaje de señas

            if (letra != '-' && letra != ultima_letra) { //	Comprueba si se detectó una nueva letra válida
                printf("P:%d I:%d M:%d A:%d Me:%d | IMU: %s\n", p, i, m, a, me, imu_mov ? "MOV" : "QUIETO");
                printf("Letra detectada: %c\n", letra);
                ultima_letra = letra; //Guarda la letra detectada como la última

                uart_putc(UART_ID, letra); // <-- enviar letra por Bluetooth
                
            } else if (letra == '-' && ultima_letra != '-') {
                printf("Letra no encontrada\n");
                ultima_letra = '-';
            }
        }

        tight_loop_contents();  // 	Función del SDK de la Pico usada dentro de bucles infinitos. Reduce consumo mientras hace polling
        /*es un marcador semántico que le dice al compilador y al sistema: “Este bucle no hace nada más que girar esperando eventos. 
        Puedes optimizar el consumo de energía”, si esto no estuviera El procesador podría quedarse en un ciclo ocupado constantemente, consumiendo más energía.*/


    }
}
