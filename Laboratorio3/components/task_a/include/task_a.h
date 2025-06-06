#ifndef TASK_A_H_ // CAMBIAR DE LED_RGB_H_ a TASK_A_H_
#define TASK_A_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "led_RGB.h" // Asegúrate de que esta ruta sea correcta si led_RGB.h está en otro componente
#include "uart_manager.h" // Si es necesaria

// Declara aquí el prototipo de tu función task_a
void task_a(void *pvParameters);

#endif // TASK_A_H_
