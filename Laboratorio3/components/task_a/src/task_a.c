#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "led_rgb.h"

extern SemaphoreHandle_t color_semaphore;
extern led_rgb_evento_t current_color;

void task_a(void *pvParameters) {
    while (1) {
        xSemaphoreTake(color_semaphore, portMAX_DELAY);
        led_rgb_set_event(current_color);  // Enciende con el color actual
        xSemaphoreGive(color_semaphore);
        vTaskDelay(pdMS_TO_TICKS(500));
        
        led_rgb_set_event(LED_EVENT_APAGAR);  // Apaga (sin semáforo)
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}