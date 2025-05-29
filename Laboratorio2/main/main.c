#include <stdio.h>
#include <stdbool.h>

#include "touch_buttons.h"
#include "wifi_APSTA.h"
#include "web_service.h"
#include "led_rgb.h"

#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern int led_mode_web;
static const char *TAG = "MAIN";
void app_main(void)
{
    esp_log_level_set("LED_RGB", ESP_LOG_NONE);
    esp_log_level_set("ws2812", ESP_LOG_NONE);
    // Inicializaciones
    touch_polling_init();
    led_rgb_inicializar();
    wifi_apsta_inicializar();
    web_service_inicializar();
    // Configuración del watchdog para la tarea actual (app_main)
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = 5000,
        .idle_core_mask = (1 << 0),  // Solo CPU0
        .trigger_panic = true
    };
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));  // Registra esta tarea

    // Bucle principal (round robin sin RTOS)
    while (true) {
        esp_task_wdt_reset();           // Alimenta al watchdog
        touch_polling_bucle();
        web_service_bucle();  // Se setea desde la web
        led_rgb_bucle();  // Actualiza el LED si hubo un cambio
        vTaskDelay(pdMS_TO_TICKS(10));  // Espera 10 ms
    }
}