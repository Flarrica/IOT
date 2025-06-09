#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "uart_manager.h"
#include "task_b.h"
#include "esp_log.h"

extern QueueHandle_t command_queue;

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
    // 🔇 Silenciar logs por UART para evitar interferencia
    esp_log_level_set("*", ESP_LOG_NONE);  

    uart_init();  // UART0 (USB), crea command_queue

    xTaskCreate(task_b, "task_b", 4096, NULL, 10, NULL);               // UART → command_queue
    xTaskCreate(debug_consumer, "debug_consumer", 2048, NULL, 5, NULL); // Muestra lo recibido
}