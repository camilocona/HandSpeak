#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <string.h>

#define PIN_PULGAR 2
#define PIN_INDICE 3
#define PIN_MEDIO 4
#define PIN_ANULAR 5
#define PIN_MENIQUE 6

#define I2C_SDA 8
#define I2C_SCL 9
#define LIS3DH_ADDR 0x18 // Dirección I2C GY-511

// Prototipos
char detectar_letra(uint8_t p, uint8_t i, uint8_t m, uint8_t a, uint8_t me, float ax, float ay, float az);
bool leer_acelerometro(float *ax, float *ay, float *az);

int main() {
    stdio_init_all();

    // Inicializar GPIOs
    for (int pin = PIN_PULGAR; pin <= PIN_MENIQUE; pin++) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
    }

    // Inicializar I2C
    i2c_init(i2c0, 400 * 1000); // 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Activar LIS3DH (modo normal)
    uint8_t config[] = {0x20, 0x57}; // CTRL_REG1: 0x57 = 100Hz, all axes enabled
    i2c_write_blocking(i2c0, LIS3DH_ADDR, config, 2, false);

    while (true) {
        uint8_t p = gpio_get(PIN_PULGAR);
        uint8_t i = gpio_get(PIN_INDICE);
        uint8_t m = gpio_get(PIN_MEDIO);
        uint8_t a = gpio_get(PIN_ANULAR);
        uint8_t me = gpio_get(PIN_MENIQUE);

        float ax = 0, ay = 0, az = 0;
        leer_acelerometro(&ax, &ay, &az);

        char letra = detectar_letra(p, i, m, a, me, ax, ay, az);
        if (letra != '-') {
            printf("Letra detectada: %c\n", letra);
        }

        sleep_ms(300);
    }
}

// Leer acelerómetro GY-511 (solo eje X, Y, Z)
bool leer_acelerometro(float *ax, float *ay, float *az) {
    uint8_t reg = 0x28 | 0x80; // OUT_X_L + auto-increment
    uint8_t data[6];

    if (i2c_write_blocking(i2c0, LIS3DH_ADDR, &reg, 1, true) < 0) return false;
    if (i2c_read_blocking(i2c0, LIS3DH_ADDR, data, 6, false) < 0) return false;

    int16_t raw_x = (data[1] << 8) | data[0];
    int16_t raw_y = (data[3] << 8) | data[2];
    int16_t raw_z = (data[5] << 8) | data[4];

    *ax = raw_x / 16384.0;
    *ay = raw_y / 16384.0;
    *az = raw_z / 16384.0;
    return true;
}

// Detectar letra con combinación + acelerómetro
char detectar_letra(uint8_t p, uint8_t i, uint8_t m, uint8_t a, uint8_t me, float ax, float ay, float az) {
    // Umbral simple de movimiento para las letras con "X"
    bool movimiento = (ax > 0.3 || ay > 0.3 || az > 0.3);

    if (p==0 && i==0 && m==0 && a==0 && me==0) return 'A';
    if (p==0 && i==1 && m==1 && a==1 && me==1) return movimiento ? 'B' : '-';
    if (p==0 && i==1 && m==1 && a==1 && me==1) return 'C';
    if (p==0 && i==1 && m==0 && a==0 && me==0) return 'D';
    if (p==1 && i==0 && m==0 && a==0 && me==0) return movimiento ? 'E' : '-';
    if (p==1 && i==0 && m==1 && a==1 && me==1) return 'F';
    if (p==1 && i==1 && m==0 && a==0 && me==0) return movimiento ? 'G' : '-';
    if (p==1 && i==1 && m==1 && a==0 && me==0) return movimiento ? 'H' : '-';
    if (p==0 && i==0 && m==0 && a==0 && me==1) return 'I';
    if (p==0 && i==0 && m==0 && a==0 && me==1) return movimiento ? 'J' : '-';
    if (p==1 && i==1 && m==1 && a==0 && me==0) return 'K';
    if (p==1 && i==1 && m==0 && a==0 && me==0) return 'L';
    if (p==0 && i==1 && m==1 && a==1 && me==0) return movimiento ? 'M' : '-';
    if (p==0 && i==1 && m==1 && a==0 && me==0) return 'N';
    if (p==0 && i==0 && m==0 && a==0 && me==0) return movimiento ? 'O' : '-';
    if (p==0 && i==1 && m==0 && a==1 && me==1) return 'P';
    if (p==1 && i==1 && m==1 && a==1 && me==1) return 'Q';
    if (p==1 && i==0 && m==1 && a==0 && me==0) return 'R';
    if (p==0 && i==1 && m==0 && a==0 && me==0) return movimiento ? 'S' : '-';
    if (p==1 && i==0 && m==0 && a==0 && me==0) return 'T';
    if (p==1 && i==1 && m==0 && a==0 && me==1) return 'U';
    if (p==0 && i==1 && m==1 && a==0 && me==0) return 'V';
    if (p==0 && i==1 && m==1 && a==1 && me==0) return 'W';
    if (p==1 && i==0 && m==0 && a==0 && me==1) return movimiento ? 'X' : '-';
    if (p==1 && i==0 && m==0 && a==0 && me==1) return 'Y';
    if (p==0 && i==0 && m==1 && a==1 && me==1) return 'Z';
    if (p==1 && i==1 && m==1 && a==1 && me==1) return movimiento ? ' ' : '-';

    return '-';
}