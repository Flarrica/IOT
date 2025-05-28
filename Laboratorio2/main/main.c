#include <stdio.h>
#include <stdbool.h>

#include "touch_buttons.h"
#include "touch_element/touch_element.h"
#include "wifi_APSTA.h"
#include "web_service.h"
#include "led_rgb.h"

#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern int led_mode_web;
extern int led_mode_touch;

void app_main(void)
{
    esp_log_level_set("LED_RGB", ESP_LOG_NONE);
    esp_log_level_set("ws2812", ESP_LOG_NONE);
    // Inicializaciones
    touch_buttons_init();
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
        led_rgb_set_event((led_rgb_evento_t) led_mode_web);  // Se setea desde la web
        led_rgb_bucle();  // Actualiza el LED si hubo un cambio
        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 1 s
        
        if (touch_button_pressed(1)) led_mode_touch  = LED_EVENT_ROJO;
        if (touch_button_pressed(2)) led_mode_touch  = LED_EVENT_VERDE;
        if (touch_button_pressed(3)) led_mode_touch  = LED_EVENT_AZUL;
        if (touch_button_pressed(5)) led_mode_touch  = LED_EVENT_BLANCO;
        if (touch_button_pressed(6)) led_mode_touch  = LED_EVENT_AMARILLO;
        if (touch_button_pressed(11)) led_mode_touch = LED_EVENT_CIAN;
        if (touch_button_pressed(14)) led_mode_touch = LED_EVENT_APAGAR;

        led_rgb_set_event((led_rgb_evento_t) led_mode_touch);

        led_rgb_bucle();  // Aplica el color según evento
        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 1 s
    }
}