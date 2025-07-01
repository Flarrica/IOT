#ifndef TASK_A_H_
#define TASK_A_H_

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "shared_lib.h"

typedef struct {
    led_rgb_evento_t color;
    uint32_t duration; // en segundos
} blink_info_t;

void task_a(void *pvParameters);

#endif // TASK_A_H_