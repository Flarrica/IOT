#ifndef TASK_B_H_
#define TASK_B_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"    // si es necesario

#include "driver/uart.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "led_rgb.h"

// Definición del tamaño del buffer de recepción UART
#define UART_RX_BUF_SIZE 256 // Define un tamaño adecuado
extern QueueHandle_t xQueue_led; // DECLARACIÓN para que otros archivos sepan que existe

// Definición de la estructura de comando UART
typedef struct {
    led_rgb_evento_t color; // Usamos el tipo específico del enum
    uint32_t delay_seconds;
} uart_command_t;

// Esta es la DECLARACIÓN EXTERNA de la cola
extern QueueHandle_t command_queue;

// Declaración de la función de la tarea
void task_b(void *pvParameters);

// Declaración de las funciones
void uart_init(void);
void uart_send_string(const char *str);
void uart_read_command(uart_command_t *cmd);

#endif // TASK_B_H_