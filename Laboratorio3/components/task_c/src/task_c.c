#include "task_a.h"
#include "task_b.h"
#include "task_c.h"


static const char *TAG = "TASK_C";

void task_c_init(void) {
    color_semaphore = xSemaphoreCreateMutex();
    if (color_semaphore == NULL) {
        ESP_LOGE(TAG, "No se pudo crear el semáforo");
    }
}

void task_c(void *pvParameters) {
    uart_command_t *cmd;
    while (1) {
        if (xQueueReceive(command_queue, &cmd, portMAX_DELAY)) {
            blink_info_t *info = malloc(sizeof(blink_info_t));
            if (info == NULL) {
                ESP_LOGE(TAG, "Fallo malloc para blink_info_t");
                continue;
            }

            info->color = cmd->color;
            info->duration = cmd->delay_seconds;

            xTaskCreate(blink_for_duration_task, "blink_for_duration_task", 2048, info, 8, NULL);
            free(cmd);
        }
    }
}