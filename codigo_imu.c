#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>

#define I2C_PORT i2c1
#define SDA_PIN 26
#define SCL_PIN 27
#define MPU9250_ADDR 0x68

#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B

void mpu9250_init() {
    uint8_t buf[] = {PWR_MGMT_1, 0x00}; // Despertar el MPU9250
    i2c_write_blocking(I2C_PORT, MPU9250_ADDR, buf, 2, false);
}

void leer_acelerometro(int16_t *ax, int16_t *ay) {
    uint8_t reg = ACCEL_XOUT_H;
    uint8_t datos[4]; // AX_H, AX_L, AY_H, AY_L

    i2c_write_blocking(I2C_PORT, MPU9250_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU9250_ADDR, datos, 4, false);

    *ax = (int16_t)((datos[0] << 8) | datos[1]);
    *ay = (int16_t)((datos[2] << 8) | datos[3]);
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("Iniciando MPU9250\n");

    i2c_init(I2C_PORT, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    mpu9250_init();

    while (1) {
        int16_t ax, ay;
        leer_acelerometro(&ax, &ay);
        printf("AX = %d\tAY = %d\n", ax, ay);
        sleep_ms(500);
    }
}

Tú, 1 min

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include <stdint.h>

// Pines para I2C
#define I2C_PORT i2c1
#define SDA_PIN 26
#define SCL_PIN 27

// Dirección I2C del LIS3DH (GY-511)
#define LIS3DH_ADDR 0x18

// Registros del LIS3DH
#define WHO_AM_I     0x0F
#define CTRL_REG1    0x20
#define CTRL_REG4    0x23
#define OUT_X_L      0x28  // (Lectura debe ser con auto-incremento)

// Inicializa el LIS3DH en modo normal, 100 Hz, ejes XYZ habilitados
void lis3dh_init() {
    // CTRL_REG1: ODR = 100 Hz, todos los ejes habilitados → 0x57
    uint8_t ctrl1[] = {CTRL_REG1, 0x57};
    i2c_write_blocking(I2C_PORT, LIS3DH_ADDR, ctrl1, 2, false);

    // CTRL_REG4: Escala ±2g, alta resolución → 0x08
    uint8_t ctrl4[] = {CTRL_REG4, 0x08};
    i2c_write_blocking(I2C_PORT, LIS3DH_ADDR, ctrl4, 2, false);
}

// Lee los valores de aceleración en X e Y (con auto-incremento)
void leer_aceleracion(int16_t* ax, int16_t* ay) {
    uint8_t reg = OUT_X_L | 0x80;  // Activar auto-incremento
    uint8_t datos[4];  // X_L, X_H, Y_L, Y_H

    i2c_write_blocking(I2C_PORT, LIS3DH_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, LIS3DH_ADDR, datos, 4, false);

    // Combinar bytes (sensor entrega en formato little-endian)
    *ax = (int16_t)((datos[1] << 8) | datos[0]) >> 4;  // 12-bit
    *ay = (int16_t)((datos[3] << 8) | datos[2]) >> 4;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("Iniciando LIS3DH (GY-511)\n");

    // Inicializar I2C
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Verificar si el sensor responde
    uint8_t reg = WHO_AM_I;
    uint8_t id = 0;
    i2c_write_blocking(I2C_PORT, LIS3DH_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, LIS3DH_ADDR, &id, 1, false);
    printf("WHO_AM_I = 0x%02X (esperado 0x33)\n", id);

    // Inicializar el LIS3DH
    lis3dh_init();

    // Loop principal
    while (true) {
        int16_t ax, ay;
        leer_aceleracion(&ax, &ay);
        printf("Aceleración X = %d, Y = %d\n", ax, ay);
        sleep_ms(500);
    }
}
