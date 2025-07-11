#include "task_b.h"
#include "shared_lib.h"
#include "led_rgb.h" // Para usar: led_rgb_string_to_color

#define UART_PORT UART_NUM_0
#define BUF_SIZE 1024
#define LINE_BUF_SIZE 64

static const char *TAG = "TASK_B";

extern QueueHandle_t color_queue;

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
                color_event_t cmd;
                char color_str[10];
                uint32_t delay_val;

                if (sscanf(line_buffer, "%9[^,],%lu", color_str, &delay_val) == 2) {
                    cmd.color = led_rgb_string_to_color(color_str);
                    cmd.delay_seconds = delay_val;

                    xQueueSend(color_queue, &cmd, portMAX_DELAY);
                    ESP_LOGI("TaskB", "Comando encolado: %s, %lu", color_str, delay_val);
                } else {
                    ESP_LOGW("TaskB", "Formato inválido: %s", line_buffer);
                }
                line_pos = 0;
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