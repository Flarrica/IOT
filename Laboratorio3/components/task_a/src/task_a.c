#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "led_rgb.h"
#include "esp_mac.h"
#include "task_a.h"

SemaphoreHandle_t color_semaphore = NULL;
led_rgb_evento_t current_color = LED_EVENT_APAGAR;  // valor inicial
bool parpadeo_habilitado = false;

void blink_for_duration_task(void *param) {
    blink_info_t *info = (blink_info_t *)param;
    uint32_t total_ms = info->duration * 1000;
    uint32_t elapsed = 0;

    while (elapsed < total_ms) {
        xSemaphoreTake(color_semaphore, portMAX_DELAY);
        led_rgb_set_event(info->color);
        xSemaphoreGive(color_semaphore);

        vTaskDelay(pdMS_TO_TICKS(500));
        elapsed += 500;

        led_rgb_set_event(LED_EVENT_APAGAR);
        vTaskDelay(pdMS_TO_TICKS(500));
        elapsed += 500;
    }

    led_rgb_set_event(LED_EVENT_APAGAR);
    free(info);
    vTaskDelete(NULL);
}

void task_a(void *pvParameters) {
    while (1) {
        if (parpadeo_habilitado) {
            xSemaphoreTake(color_semaphore, portMAX_DELAY);
            led_rgb_set_event(current_color);  // Enciende
            xSemaphoreGive(color_semaphore);
            vTaskDelay(pdMS_TO_TICKS(500));

            led_rgb_set_event(LED_EVENT_APAGAR);  // Apaga
            vTaskDelay(pdMS_TO_TICKS(500));
        } else {
            vTaskDelay(pdMS_TO_TICKS(100));  // Espera antes de chequear de nuevo
        }
    }
}