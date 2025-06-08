#ifndef TASK_C_H_
#define TASK_C_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

// Declaraciones externas si son necesarias (sino, eliminarlas)
extern SemaphoreHandle_t color_semaphore;

// Declaración de la tarea
void task_c(void *pvParameter);

#endif // TASK_C_H_