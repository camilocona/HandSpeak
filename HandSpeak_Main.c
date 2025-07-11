#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
//#include "hardware/rtc.h"
#include "hardware/timer.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "hardware/uart.h"

// ------------------ UART para Bluetooth ------------------
#define UART_ID uart0
#define BAUD_RATE 9600

#define UART_TX_PIN 16
#define UART_RX_PIN 17


// ------------------ Pines de dedos ------------------
#define PIN_PULGAR 3
#define PIN_INDICE 4
#define PIN_MEDIO 5
#define PIN_ANULAR 7
#define PIN_MENIQUE 8

// ------------------ I2C para GY-91 (MPU9250 en i2c1, GP26/27) ------------------
#define I2C_PORT i2c1
#define SDA_PIN 26
#define SCL_PIN 27
#define MPU9250_ADDR 0x68
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B

// ------------------ Configuración IMU ------------------
#define IMU_UMBRAL 0.05f // umbral de aceleración en "g" para detectar movimiento

// ------------------ Bandera para polling IMU ------------------
volatile bool flag_tiempo_imu = false;

// ------------------ Timer callback (interrupción periódica) ------------------
int64_t repetir_lectura_imu(alarm_id_t id, void *user_data) {
    flag_tiempo_imu = true;
    add_alarm_in_ms(3000, repetir_lectura_imu, NULL, false); // reprogramar cada 3s
    return 0;
}

// ------------------ Inicialización MPU9250 ------------------
void mpu9250_init() {
    uint8_t buf[] = {PWR_MGMT_1, 0x00}; // Quitar modo sleep
    i2c_write_blocking(I2C_PORT, MPU9250_ADDR, buf, 2, false);
}

// ------------------ Lectura de acelerómetro ------------------
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
    sleep_ms(2000);  // Solo se deja aquí para permitir que se abra la terminal

    printf("Inicializando sistema HandSpeak + IMU\n");

    // ------------------ Inicializar GPIO de dedos ------------------
    for (int pin = PIN_PULGAR; pin <= PIN_MENIQUE; pin++) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_down(pin);
    }

    // ------------------ Inicializar IMU (i2c1 en GP26 y GP27) ------------------
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    mpu9250_init();

    // ------------------ Iniciar timer para polling+interrupción IMU ------------------
    add_alarm_in_ms(3000, repetir_lectura_imu, NULL, false);


    // ------------------ Inicializar UART para Bluetooth ------------------
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);


    char ultima_letra = '-';

    while (true) {
        // Leer estado de dedos
        uint8_t p = !!gpio_get(PIN_PULGAR);
        uint8_t i = !!gpio_get(PIN_INDICE);
        uint8_t m = !!gpio_get(PIN_MEDIO);
        uint8_t a = !!gpio_get(PIN_ANULAR);
        uint8_t me = !!gpio_get(PIN_MENIQUE);

        // Solo leer IMU y detectar letra si la bandera se activó
        if (flag_tiempo_imu) {
            flag_tiempo_imu = false;

            float ax = 0, ay = 0, az = 0;
            bool imu_mov = false;

            if (leer_acelerometro(&ax, &ay, &az)) {
                float magnitud = sqrtf(ax * ax + ay * ay + az * az);
                float delta = fabsf(magnitud - 1.0f);
                imu_mov = delta > IMU_UMBRAL;

               // printf("Magnitud: %.3f | Delta: %.3f | IMU: %s\n", magnitud, delta, imu_mov ? "MOV" : "QUIETO");
            }

            char letra = detectar_letra(p, i, m, a, me, imu_mov);

            if (letra != '-' && letra != ultima_letra) {
                //printf("P:%d I:%d M:%d A:%d Me:%d | IMU: %s\n", p, i, m, a, me, imu_mov ? "MOV" : "QUIETO");
                //printf("Letra detectada: %c\n", letra);
                ultima_letra = letra;

                uart_putc(UART_ID, letra); // <-- enviar letra por Bluetooth
                
            } else if (letra == '-' && ultima_letra != '-') {
                //printf("Letra no encontrada\n");
                ultima_letra = '-';
            }
        }

        tight_loop_contents();  // Reduce consumo mientras hace polling
    }
}