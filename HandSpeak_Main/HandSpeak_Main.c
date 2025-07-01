
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <string.h>

#define PIN_PULGAR 3
#define PIN_INDICE 4
#define PIN_MEDIO 5
#define PIN_ANULAR 7
#define PIN_MENIQUE 8

#define I2C_SDA 20
#define I2C_SCL 21
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
        gpio_pull_down(pin); // Evitar pines flotantes
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


        // Forzar valores a 0 o 1
        p = !!p;
        i = !!i;
        m = !!m;
        a = !!a;
        me = !!me;

        
 
        float ax = 0, ay = 0, az = 0;
       /*
        if (!leer_acelerometro(&ax, &ay, &az)) {
            printf("Error al leer acelerómetro\n");
            continue;
        }

        
    */  char letra = detectar_letra(p, i, m, a, me, ax, ay, az);
        if (letra != '-') {
            // Mostrar estado de los dedos
            printf("P:%d I:%d M:%d A:%d Me:%d\n", p, i, m, a, me);
            printf("Letra detectada: %c\n", letra);
        }

        sleep_ms(300);
    }
}

// Leer acelerómetro GY-511 (ejes X, Y, Z)
bool leer_acelerometro(float *ax, float *ay, float *az) {
    uint8_t reg = 0x28 | 0x80; // OUT_X_L con auto-incremento
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

// Detectar letra basada en dedos (sin movimiento)
char detectar_letra(uint8_t p, uint8_t i, uint8_t m, uint8_t a, uint8_t me, float ax, float ay, float az) {
    uint8_t cod = (p << 4) | (i << 3) | (m << 2) | (a << 1) | (me << 0);

    switch (cod) {
        case 0b00000: return 'A';
        case 0b01111: return 'C';
        case 0b01000: return 'D';
        //case 0b10000: return 'E';
        case 0b10111: return 'F';
        //case 0b11000: return 'G';
        //case 0b11100: return 'H';
        case 0b00001: return 'I';
        case 0b01110: return 'M';
        case 0b01100: return 'N';
        case 0b01010: return 'P';
        case 0b11111: return 'Q';
        case 0b10100: return 'R';
        case 0b11001: return 'U';
        case 0b10001: return 'Y';
        default: return '-';
    }
}

/*

#include "pico/stdlib.h"
#include <stdio.h>

#define PIN_PULGAR 4
//#define PIN_INDICE 3
//#define PIN_MEDIO 4
//#define PIN_ANULAR 5
#define PIN_MENIQUE 5

int main() {
    stdio_init_all();

    gpio_init(PIN_PULGAR);
    gpio_set_dir(PIN_PULGAR, GPIO_IN);
    gpio_pull_down(PIN_PULGAR);

    gpio_init(PIN_MENIQUE);
    gpio_set_dir(PIN_MENIQUE, GPIO_IN);
    gpio_pull_down(PIN_MENIQUE);

    while (true) {
        uint8_t p = gpio_get(PIN_PULGAR);
        uint8_t me = gpio_get(PIN_MENIQUE);

        printf("Pulgar: %d | Meñique: %d\n", p, me);
        sleep_ms(300);
    }
} */