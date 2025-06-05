#include "freertos/queue.h"
#include "uart_manager.h"

typedef struct {
    led_rgb_evento_t color;  // Usa tu tipo de evento
    uint32_t delay_seconds;
} uart_command_t;

QueueHandle_t command_queue;

void task_b(void *pvParameters) {
    uart_command_t received_cmd;
    char color_str[20];
    while (1) {
        // Ejemplo de parser (ajusta según tu UART)
        uart_read_string(color_str, sizeof(color_str));  // "Rojo 10" -> "Rojo"
        uint32_t delay;
        sscanf(uart_read_delay(), "%u", &delay);  // "10" -> 10

        // Mapear string a led_rgb_evento_t
        if (strcmp(color_str, "Rojo") == 0) received_cmd.color = LED_EVENT_ROJO;
        else if (strcmp(color_str, "Verde") == 0) received_cmd.color = LED_EVENT_VERDE;
        // ... otros colores

        received_cmd.delay_seconds = delay;
        xQueueSend(command_queue, &received_cmd, portMAX_DELAY);
    }
}