#ifndef TASK_C_H_
#define TASK_C_H_

#include <stdbool.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "led_rgb.h"
#include "esp_log.h"


extern QueueHandle_t command_queue;
extern SemaphoreHandle_t color_semaphore;
extern led_rgb_evento_t current_color;
extern bool parpadeo_habilitado;

void task_c_init(void);
void task_c(void *pvParameters);

#endif // TASK_C_H_