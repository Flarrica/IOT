#ifndef TASK_A_H_
#define TASK_A_H_

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_rgb.h"

extern SemaphoreHandle_t color_semaphore;
extern bool parpadeo_habilitado;
extern led_rgb_evento_t current_color;

typedef struct {
    led_rgb_evento_t color;
    uint32_t duration; // en segundos
} blink_info_t;

// Declaración de la función de la tarea
void task_a(void *pvParameters);
void blink_for_duration_task(void *param);

#endif // TASK_A_H_