#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "led_rgb.h"


extern SemaphoreHandle_t color_semaphore;
extern led_rgb_evento_t current_color;
extern bool parpadeo_habilitado;

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