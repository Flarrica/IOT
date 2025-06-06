#ifndef TASK_B_H_ // CAMBIAR DE LED_RGB_H_ a TASK_B_H_
#define TASK_B_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "uart_manager.h" // Si es necesaria

// Declara aquí el prototipo de tu función task_b
void task_b(void *pvParameters);

#endif // TASK_B_H_