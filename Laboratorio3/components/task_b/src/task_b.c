#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "uart_manager.h"
#include "led_rgb.h"

extern QueueHandle_t command_queue;  // Cola con estructura completa

void task_b(void *pvParameters) {
    uart_command_t received_cmd;

    while (1) {
        uart_read_command(&received_cmd);

        if (xQueueSend(command_queue, &received_cmd, portMAX_DELAY) != pdPASS) {
            ESP_LOGE("TaskB", "Error al enviar comando a la cola");
        } else {
            ESP_LOGI("TaskB", "Comando encolado: color=%d, delay=%lu",
                received_cmd.color, (unsigned long)received_cmd.delay_seconds);
        }

        // No se aplica delay aquí, eso lo maneja task_c con timer
    }
}