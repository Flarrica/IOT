#ifndef TASK_C_H_
#define TASK_C_H_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "led_rgb.h"
#include <stdbool.h>

extern SemaphoreHandle_t color_semaphore;
extern led_rgb_evento_t current_color;
extern bool parpadeo_habilitado;

void task_c_init(void);
void task_c(void *pvParameters);

#endif // TASK_C_H_