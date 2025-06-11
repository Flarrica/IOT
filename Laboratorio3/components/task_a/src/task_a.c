#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "led_rgb.h"
#include "esp_mac.h"
#include "task_a.h"

void task_a(void *pvParameters) {
    while (1) {
        // Espero hasta que task_c me diga que hay un nuevo color
        xSemaphoreTake(color_semaphore, portMAX_DELAY);

        while (1) {
            // Parpadeo indefinidamente hasta que haya un nuevo color
            led_rgb_set_event(current_color);  // Enciende
            vTaskDelay(pdMS_TO_TICKS(500));

            led_rgb_set_event(LED_EVENT_APAGAR);  // Apaga
            vTaskDelay(pdMS_TO_TICKS(500));

            // Verifico si color_semaphore tiene un nuevo evento pendiente
            // No bloqueo si no hay semáforo disponible
            if (xSemaphoreTake(color_semaphore, 0) == pdTRUE) {
                // Hay nuevo color disponible, actualizo y reinicio parpadeo
                break;
            }
        }
    }
}