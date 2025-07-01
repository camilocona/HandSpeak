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

char detectar_letra(uint8_t p, uint8_t i, uint8_t m, uint8_t a, uint8_t me);

int main() {
    stdio_init_all();

    // Inicializar GPIOs
    for (int pin = PIN_PULGAR; pin <= PIN_MENIQUE; pin++) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_down(pin); // Evitar pines flotantes
    }

    char ultima_letra = '-';

    while (true) {
        uint8_t p = !!gpio_get(PIN_PULGAR);
        uint8_t i = !!gpio_get(PIN_INDICE);
        uint8_t m = !!gpio_get(PIN_MEDIO);
        uint8_t a = !!gpio_get(PIN_ANULAR);
        uint8_t me = !!gpio_get(PIN_MENIQUE);

        char letra = detectar_letra(p, i, m, a, me);

        if (letra != '-' && letra != ultima_letra) {
            printf("P:%d I:%d M:%d A:%d Me:%d\n", p, i, m, a, me);
            printf("Letra detectada: %c\n", letra);
            ultima_letra = letra;
        } else if (letra == '-' && ultima_letra != '-') {
            printf("Letra no encontrada\n");
            ultima_letra = '-';
        }

        sleep_ms(3000); // Espera de 1 segundo entre lecturas
    }
}

char detectar_letra(uint8_t p, uint8_t i, uint8_t m, uint8_t a, uint8_t me) {
    uint8_t cod = (p << 4) | (i << 3) | (m << 2) | (a << 1) | me;

    switch (cod) {
        case 0b10000: return 'A';
        case 0b00000: return 'C';
        case 0b01000: return 'D';
        case 0b10111: return 'F';
        case 0b00001: return 'I';
        case 0b11100: return 'K';
        case 0b11000: return 'L';
        case 0b01011: return 'P';
        case 0b10100: return 'R';
        case 0b11001: return 'U';
        case 0b01100: return 'V';
        case 0b01110: return 'W';
        case 0b10001: return 'Y';
        case 0b00111: return 'Z';
        case 0b11111: return 'Espacio'; 
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
