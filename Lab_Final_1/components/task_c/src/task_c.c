#include "task_c.h"
#include "shared_lib.h"

static const char *TAG = "TASK_C";

typedef struct {
    led_rgb_evento_t color;
} timer_info_t;

//antes del callback
const char* color_to_string(led_rgb_evento_t color) {
    switch (color) {
        case LED_EVENT_ROJO:     return "Rojo";
        case LED_EVENT_VERDE:    return "Verde";
        case LED_EVENT_AZUL:     return "Azul";
        case LED_EVENT_CIAN:     return "Cian";
        case LED_EVENT_AMARILLO: return "Amarillo";
        case LED_EVENT_BLANCO:   return "Blanco";
        case LED_EVENT_APAGAR:   return "Apagar";
        default:                 return "Desconocido";
    }
}

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
    color_event_t cmd;

    while (1) {
        if (xQueueReceive(command_queue, &cmd, portMAX_DELAY) == pdTRUE) {

            timer_info_t *info = malloc(sizeof(timer_info_t));
            if (info == NULL) {
                ESP_LOGE(TAG, "No se pudo alocar memoria para timer_info_t");
                continue;
            }

            info->color = cmd.color;
            uint32_t delay_ticks = pdMS_TO_TICKS(cmd.delay_seconds * 1000);

            if (delay_ticks == 0) {
                // Ejecutar directamente la lógica del callback
                current_color = info->color;
                xSemaphoreGive(color_semaphore);
                ESP_LOGI(TAG, "Prende %s inmediatamente", color_to_string(info->color));
                free(info);
                continue;
            }

            TimerHandle_t timer = xTimerCreate(
                "StartBlinkTimer",
                delay_ticks,
                pdFALSE, // One-shot
                info,
                vTimerCallback
            );

            if (timer != NULL) {
                xTimerStart(timer, 0);
                ESP_LOGI(TAG, "Prende %s en %.1f seg.", color_to_string(info->color), (double)cmd.delay_seconds);
            } else {
                ESP_LOGE(TAG, "Error al crear el timer");
                free(info); // Liberar si falla
            }
        }
    }
}