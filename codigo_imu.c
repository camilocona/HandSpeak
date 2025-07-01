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
