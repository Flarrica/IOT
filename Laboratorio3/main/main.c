#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "uart_manager.h"
#include "task_c.h"
#include "esp_log.h"

// Declaración externa de la cola de comandos
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
    // Silenciar logs globales para evitar conflicto con UART
    //esp_log_level_set("*", ESP_LOG_NONE);
    esp_log_level_set("TASK_C", ESP_LOG_INFO);
    esp_log_level_set("UART_MGR", ESP_LOG_INFO);

    // Inicializar UART y colas
    uart_init();
    task_c_init();
    led_rgb_inicializar();

    xTaskCreate(task_c, "task_c", 4096, NULL, 10, NULL);
    xTaskCreate(debug_consumer, "debug_consumer", 2048, NULL, 5, NULL);
}