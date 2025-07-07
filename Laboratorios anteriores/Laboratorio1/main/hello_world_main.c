#include <stdio.h>
#include "led_rgb.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_delay.h"
#include "esp_task_wdt.h" 


void app_main(void)
{

    printf("Iniciando programa LED RGB...\n");

    led_rgb_inicializar();  // Configura RMT y led_strip

    while (1) {
        /*led_rgb_prendo_ROJO();
        delay_ms(500);
        led_rgb_prendo_AZUL();
        delay_ms(500);
        led_rgb_apagar();
        delay_ms(500);*/

        led_rgb_prendo_ROJO();
        delay_ms(500);
        led_rgb_apagar();

        led_rgb_prendo_AZUL();
        delay_ms(500);
        led_rgb_apagar();
        
        led_rgb_prendo_VERDE();
        delay_ms(500);
        led_rgb_apagar();

        led_rgb_prendo_BLANCO();
        delay_ms(500);
        led_rgb_apagar();
        delay_ms(500);
    }
}