#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include "uart_manager.h"
#include "led_rgb.h"

#define UART_PORT UART_NUM_0
#define BUF_SIZE 1024
#define LINE_BUF_SIZE 64  // Tamaño máximo de línea esperada

QueueHandle_t command_queue;

static const char *TAG = "UART_MGR";

// Nueva tarea de lectura caracter a caracter
static void uart_read_line_task(void *pvParameters) {
    uint8_t c;
    static char line_buffer[LINE_BUF_SIZE];
    static int line_pos = 0;

    while (1) {
        int len = uart_read_bytes(UART_PORT, &c, 1, portMAX_DELAY);
        if (len > 0) {
            if (c == '\n' || c == '\r') {
                if (line_pos > 0) {
                    line_buffer[line_pos] = '\0';

                    uart_command_t cmd;
                    char color_str[10];
                    uint32_t delay_val;

                    if (sscanf(line_buffer, "%9[^,],%lu", color_str, &delay_val) == 2) {
                        if (strcmp(color_str, "rojo") == 0) {
                            cmd.color = LED_EVENT_ROJO;
                        } else if (strcmp(color_str, "verde") == 0) {
                            cmd.color = LED_EVENT_VERDE;
                        } else if (strcmp(color_str, "azul") == 0) {
                            cmd.color = LED_EVENT_AZUL;
                        } else if (strcmp(color_str, "apagar") == 0) {
                            cmd.color = LED_EVENT_APAGAR;
                        } else {
                            ESP_LOGW(TAG, "Color desconocido: %s", color_str);
                            cmd.color = LED_EVENT_APAGAR;
                        }
                        cmd.delay_seconds = delay_val;
                        xQueueSend(command_queue, &cmd, portMAX_DELAY);
                        ESP_LOGI(TAG, "Comando válido recibido: %s, %lu", color_str, delay_val);
                    } else {
                        ESP_LOGW(TAG, "Formato inválido: %s", line_buffer);
                    }

                    line_pos = 0;
                }
            } else if (line_pos < LINE_BUF_SIZE - 1) {
                line_buffer[line_pos++] = c;
            } else {
                line_pos = 0;
                ESP_LOGW(TAG, "Línea demasiado larga, descartada");
            }
        }
    }
}

void uart_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                    UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    ESP_LOGI(TAG, "UART inicializado correctamente");

    command_queue = xQueueCreate(10, sizeof(uart_command_t));
    if (command_queue == NULL) {
        ESP_LOGE(TAG, "Error al crear command_queue");
    } else {
        ESP_LOGI(TAG, "command_queue creada exitosamente");
    }

    xTaskCreate(uart_read_line_task, "uart_read_line_task", 3072, NULL, 10, NULL);
}