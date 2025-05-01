#include <stdio.h>
#include "led_rgb.h"
#include "led_delay.h"

void app_main(void)
{
    printf("Iniciando programa LED RGB...\n");

    led_rgb_inicializar();  // Configura RMT y led_strip

    while (1) {
        led_rgb_prendo_ROJO();
        delay_ms(500);

        led_rgb_apagar();
        delay_ms(500);
    }
}