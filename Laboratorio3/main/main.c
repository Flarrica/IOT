#include <stdio.h>
#include <stdbool.h>

#include "touch_buttons.h"
#include "wifi_APSTA.h"
#include "web_service.h"
#include "led_rgb.h"
#include "delay.h"

#include "esp_log.h"
#include "esp_rom_sys.h"

void app_main(void)
{
    esp_log_level_set("LED_RGB", ESP_LOG_NONE);
    esp_log_level_set("ws2812", ESP_LOG_NONE);
    esp_log_level_set("DELAYS", ESP_LOG_NONE);
    // Inicializaciones
    touch_polling_init();
    led_rgb_inicializar();
    wifi_apsta_inicializar();
    web_service_inicializar();
    // Configuración del watchdog para la tarea actual (app_main)
    watchdog_inicializar();
    // Bucle principal (round robin sin RTOS)
 
    while (true) {

        touch_polling_bucle();

        web_service_bucle();

        led_rgb_bucle();  // Actualiza el LED si hubo un cambio

        delay_ms(10);  // Espera 10 ms
        
        watchdog_resetear();           // Alimenta al watchdog
    }
}