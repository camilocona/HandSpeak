#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define ADC_PIN 26           // GPIO26 = ADC0
#define R_REF 10000.0f       // Resistencia de referencia en ohms (10k)
#define VREF 3.3f            // Voltaje de referencia

int main() {
    stdio_init_all();       // Inicializa UART para printf
    adc_init();             // Inicializa ADC
    adc_gpio_init(ADC_PIN); // Habilita el pin GPIO26 como ADC
    adc_select_input(0);    // Selecciona ADC0

    while (true) {
        uint16_t raw = adc_read();                 // Lectura cruda (12 bits: 0-4095)
        float v_adc = raw * VREF / 4095.0f;        // Convertir a voltaje
        float r_x = R_REF * (v_adc / (VREF - v_adc)); // Calcular Rx

        printf("ADC Raw: %d\t V_ADC: %.3f V\t R_x: %.2f ohms\n", raw, v_adc, r_x);
        sleep_ms(500);
    }
}
