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

#include "shared_lib.h"


// Definición del tamaño del buffer de recepción UART
#define UART_RX_BUF_SIZE 256 // Define un tamaño adecuado

// Declaración de la función de la tarea
void task_b(void *pvParameters);

#endif // TASK_B_H_