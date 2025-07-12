
 /**
 * @file handspeak_main.c
 * @brief Programa principal para el proyecto HandSpeak en Raspberry Pi Pico.
 *
 * @details
 * Este programa lee los estados de los dedos a través de pines GPIO,
 * usa un MPU9250 vía I2C para detectar movimiento con la IMU,
 * y envía letras codificadas por UART al módulo Bluetooth HC-05.
 *
 * Proyecto adaptado para usar con el SDK de Raspberry Pi Pico.
 *
 * @authors
 * - Maria Valentina Quiroga Alzate
 * - Mario Andrés Leal Galvis
 * - Simón Llano Cárdenas
 * - Camilo Andrés Anacona Anacona
 */


#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "hardware/uart.h"

// ------------------ UART para Bluetooth ------------------

#define UART_ID uart0         /**< UART usada para Bluetooth */
#define BAUD_RATE 9600         /**< Velocidad de UART en baudios */
#define UART_TX_PIN 16         /**< Pin GPIO para TX */
#define UART_RX_PIN 17         /**< Pin GPIO para RX */

// ------------------ Pines de dedos ------------------

#define PIN_PULGAR 3           /**< GPIO para dedo pulgar */
#define PIN_INDICE 4           /**< GPIO para dedo índice */
#define PIN_MEDIO 5            /**< GPIO para dedo medio */
#define PIN_ANULAR 7           /**< GPIO para dedo anular */
#define PIN_MENIQUE 8          /**< GPIO para dedo meñique */

// ------------------ I2C para GY-91 (MPU9250) ------------------

#define I2C_PORT i2c1
#define SDA_PIN 26              /**< GPIO para SDA */
#define SCL_PIN 27              /**< GPIO para SCL */
#define MPU9250_ADDR 0x68       /**< Dirección I2C del MPU9250 */
#define PWR_MGMT_1 0x6B         /**< Registro de power management */
#define ACCEL_XOUT_H 0x3B       /**< Registro de datos del acelerómetro */

// ------------------ Configuración IMU ------------------

#define IMU_UMBRAL 0.05f        /**< Umbral en 'g' para detectar movimiento */

// ------------------ Bandera para polling IMU ------------------

volatile bool flag_tiempo_imu = false; /**< Bandera activada por timer */

// ------------------ Timer callback (interrupción periódica) ------------------

/**
 * @brief Callback del timer para activar la lectura de la IMU.
 *
 * Programa una nueva alarma cada 3 segundos.
 *
 * @param id ID de la alarma
 * @param user_data Usado 
 * @return int64_t Siempre retorna 0
 */
int64_t repetir_lectura_imu(alarm_id_t id, void *user_data) {
    flag_tiempo_imu = true;
    add_alarm_in_ms(3000, repetir_lectura_imu, NULL, false);
    return 0;
}

// ------------------ Inicialización MPU9250 ------------------

/**
 * @brief Inicializa el MPU9250 para quitarlo del modo sleep.
 */
void mpu9250_init() {
    uint8_t buf[] = {PWR_MGMT_1, 0x00};
    i2c_write_blocking(I2C_PORT, MPU9250_ADDR, buf, 2, false);
}

// ------------------ Lectura de acelerómetro ------------------

/**
 * @brief Lee los valores de aceleración en X, Y y Z del MPU9250.
 *
 * @param ax Puntero para almacenar aceleración en X (en 'g')
 * @param ay Puntero para almacenar aceleración en Y (en 'g')
 * @param az Puntero para almacenar aceleración en Z (en 'g')
 * @return true si la lectura fue exitosa, false si falló.
 */
bool leer_acelerometro(float *ax, float *ay, float *az) {
    uint8_t reg = ACCEL_XOUT_H;
    uint8_t datos[6];
    if (i2c_write_blocking(I2C_PORT, MPU9250_ADDR, &reg, 1, true) < 0) return false;
    if (i2c_read_blocking(I2C_PORT, MPU9250_ADDR, datos, 6, false) < 0) return false;

    int16_t raw_x = (datos[0] << 8) | datos[1];
    int16_t raw_y = (datos[2] << 8) | datos[3];
    int16_t raw_z = (datos[4] << 8) | datos[5];

    *ax = raw_x / 16384.0f;
    *ay = raw_y / 16384.0f;
    *az = raw_z / 16384.0f;
    return true;
}

// ------------------ Detección de letras ------------------

/**
 * @brief Determina la letra detectada según el estado de los dedos y movimiento.
 *
 * @param p Estado del pulgar (0 o 1)
 * @param i Estado del índice (0 o 1)
 * @param m Estado del medio (0 o 1)
 * @param a Estado del anular (0 o 1)
 * @param me Estado del meñique (0 o 1)
 * @param imu_activo true si hay movimiento detectado por la IMU
 * @return char Letra detectada, '-' si no se detecta ninguna
 */
char detectar_letra(uint8_t p, uint8_t i, uint8_t m, uint8_t a, uint8_t me, bool imu_activo) {
    uint8_t cod = (p << 4) | (i << 3) | (m << 2) | (a << 1) | me;

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
        case 0b01011: return 'P';
        case 0b10100: return 'R';
        case 0b11001: return 'U';
        case 0b00111: return 'Z';
        case 0b01111: return 'B';
        default: return '-';
    }
}

// ------------------ Programa principal ------------------

/**
 * @brief Función principal del programa HandSpeak.
 *
 * Inicializa GPIOs, I2C, IMU y UART, detecta el estado de los dedos
 * y envía la letra detectada por Bluetooth.
 *
 * @return int Nunca retorna, ciclo infinito.
 */
int main() {
    stdio_init_all();
    sleep_ms(2000);

    printf("Inicializando sistema HandSpeak + IMU\n");

    for (int pin = PIN_PULGAR; pin <= PIN_MENIQUE; pin++) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_down(pin);
    }

    i2c_init(I2C_PORT, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    mpu9250_init();

    add_alarm_in_ms(3000, repetir_lectura_imu, NULL, false);

    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    char ultima_letra = '-';

    while (true) {
        uint8_t p = !!gpio_get(PIN_PULGAR);
        uint8_t i = !!gpio_get(PIN_INDICE);
        uint8_t m = !!gpio_get(PIN_MEDIO);
        uint8_t a = !!gpio_get(PIN_ANULAR);
        uint8_t me = !!gpio_get(PIN_MENIQUE);

        if (flag_tiempo_imu) {
            flag_tiempo_imu = false;

            float ax = 0, ay = 0, az = 0;
            bool imu_mov = false;

            if (leer_acelerometro(&ax, &ay, &az)) {
                float magnitud = sqrtf(ax * ax + ay * ay + az * az);
                float delta = fabsf(magnitud - 1.0f);
                imu_mov = delta > IMU_UMBRAL;
            }

            char letra = detectar_letra(p, i, m, a, me, imu_mov);

            if (letra != '-' && letra != ultima_letra) {
                ultima_letra = letra;
                uart_putc(UART_ID, letra);
            } else if (letra == '-' && ultima_letra != '-') {
                ultima_letra = '-';
            }
        }

        tight_loop_contents();
    }
}
