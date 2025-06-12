#include "task_c.h"

static const char *TAG = "TASK_C";

typedef struct {
    led_rgb_evento_t color;
} timer_info_t;

// Este es el callback del timer
void vTimerCallback(TimerHandle_t xTimer)
{
    timer_info_t *info = (timer_info_t *)pvTimerGetTimerID(xTimer);

    // Solo aquí actualizamos el color y notificamos a task_a
    current_color = info->color;
    xSemaphoreGive(color_semaphore);

    free(info); // Liberamos la memoria
}

void task_c(void *pvParameters) {
    uart_command_t cmd;

    while (1) {
        if (xQueueReceive(command_queue, &cmd, portMAX_DELAY) == pdTRUE) {

            // Creamos estructura dinámica con el color
            timer_info_t *info = malloc(sizeof(timer_info_t));
            if (info == NULL) {
                ESP_LOGE(TAG, "No se pudo alocar memoria para timer_info_t");
                continue;
            }

            info->color = cmd.color;

            TimerHandle_t timer = xTimerCreate(
                "StartBlinkTimer",
                pdMS_TO_TICKS(cmd.delay_seconds * 1000),
                pdFALSE, // One-shot
                info,
                vTimerCallback
            );

            if (timer != NULL) {
                xTimerStart(timer, 0);
            } else {
                ESP_LOGE(TAG, "Error al crear el timer");
                free(info); // Importante liberar si falla
            }
        }
    }
}