#include "task_c.h"
#include "task_b.h"

static const char *TAG = "TASK_C";

SemaphoreHandle_t color_semaphore = NULL;
led_rgb_evento_t current_color = LED_EVENT_APAGAR;  // valor inicial
bool parpadeo_habilitado = false;

typedef struct {
    led_rgb_evento_t color;
    uint32_t duration; // en segundos
} blink_info_t;

void task_c_init(void) {
    color_semaphore = xSemaphoreCreateMutex();
    if (color_semaphore == NULL) {
        ESP_LOGE("TASK_C", "No se pudo crear el semáforo");
    }
}

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

void task_c(void *pvParameters) {
    uart_command_t *cmd;
    while (1) {
        if (xQueueReceive(command_queue, &cmd, portMAX_DELAY)) {
            blink_info_t *info = malloc(sizeof(blink_info_t));
            if (info == NULL) {
                ESP_LOGE("TASK_C", "Fallo malloc para blink_info_t");
                continue;
            }

            info->color = cmd->color;
            info->duration = cmd->delay_seconds;

            xTaskCreate(blink_for_duration_task, "blink_for_duration_task", 2048, info, 8, NULL);
            free(cmd);
        }
    }
}