#include "task_b.h"

#define UART_PORT UART_NUM_0
#define BUF_SIZE 1024
#define LINE_BUF_SIZE 64

static const char *TAG = "TASK_B";

// Nueva tarea de lectura caracter a caracter
void task_b(void *pvParameters) {
    uint8_t c;
    static char line_buffer[LINE_BUF_SIZE];
    static int line_pos = 0;

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
    ESP_LOGI(TAG, "TaskB - UART configurado");

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
                        if (strcasecmp(color_str, "rojo") == 0) {
                            cmd.color = LED_EVENT_ROJO;
                        } else if (strcasecmp(color_str, "verde") == 0) {
                            cmd.color = LED_EVENT_VERDE;
                        } else if (strcasecmp(color_str, "azul") == 0) {
                            cmd.color = LED_EVENT_AZUL;
                        } else if (strcasecmp(color_str, "amarillo") == 0) {
                            cmd.color = LED_EVENT_AMARILLO;
                        } else if (strcasecmp(color_str, "cian") == 0) {
                            cmd.color = LED_EVENT_CIAN;
                            } else if (strcasecmp(color_str, "blanco") == 0) {
                            cmd.color = LED_EVENT_BLANCO;
                        } else if (strcasecmp(color_str, "apagar") == 0) {
                            cmd.color = LED_EVENT_APAGAR;
                        } else {
                            ESP_LOGW("TaskB", "Color desconocido: %s", color_str);
                            cmd.color = LED_EVENT_APAGAR;
                        }

                        cmd.delay_seconds = delay_val;

                        // Enviamos el comando
                        xQueueSend(command_queue, &cmd, portMAX_DELAY);
                        ESP_LOGI("TaskB", "Comando encolado: %s, %lu", color_str, delay_val);
                    } else {
                        ESP_LOGW("TaskB", "Formato inválido: %s", line_buffer);
                    }
                    line_pos = 0;
                }
            } else {
                if (line_pos < LINE_BUF_SIZE - 1) {
                    line_buffer[line_pos++] = c;
                } else {
                    ESP_LOGW(TAG, "Línea demasiado larga, descartada");
                    line_pos = 0;
                }
            }
        }
    }
}