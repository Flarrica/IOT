#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "uart_manager.h"
#include "task_b.h"
#include "led_rgb.h"

// Cola que recibe los comandos UART
QueueHandle_t command_queue;

void debug_consumer(void *pvParameters) {
    uart_command_t cmd;
    while (1) {
        if (xQueueReceive(command_queue, &cmd, portMAX_DELAY)) {
            printf("DEBUG >> Comando recibido: Color=%d, Delay=%lu\n", 
                   cmd.color, (unsigned long)cmd.delay_seconds);
        }
    }
}

void app_main(void) {
    uart_init();  // También podés crear la cola aquí si ya está implementado así
    command_queue = xQueueCreate(10, sizeof(uart_command_t));

    xTaskCreate(task_b, "task_b", 4096, NULL, 10, NULL);          // Escucha comandos UART
    xTaskCreate(debug_consumer, "debug_consumer", 2048, NULL, 5, NULL);  // Imprime lo que llega
}