#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t command_queue;
void task_b(void *pvParameters);