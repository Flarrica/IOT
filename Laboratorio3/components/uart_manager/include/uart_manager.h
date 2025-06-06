#ifndef UART_MANAGER_H_
#define UART_MANAGER_H_

#include "freertos/queue.h" // Necesario para QueueHandle_t
#include "esp_log.h"        // Para ESP_LOGI, ESP_LOGE, etc.
#include "driver/uart.h"    // Necesario para las funciones y tipos de UART (uart_config_t, uart_num_t, etc.)
#include "led_rgb.h"        // Para led_rgb_evento_t

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

// Declaración de las funciones
void uart_init(void);
void uart_send_string(const char *str);
void uart_read_command(uart_command_t *cmd);

#endif /* UART_MANAGER_H_ */