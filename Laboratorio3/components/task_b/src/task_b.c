#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
// No necesitas string.h ni stdio.h aquí si el parseo se hace en uart_manager.c
#include "uart_manager.h" // ¡PARA uart_command_t y uart_read_command!
#include "led_rgb.h"      // Necesario para los eventos LED_EVENT_XXX en la cola

// Declara las colas si están definidas en otro archivo (e.g., main.c)
extern QueueHandle_t xQueue_uart; // Si la usas directamente aquí
extern QueueHandle_t xQueue_led;

void task_b(void *pvParameters) {
    uart_command_t received_cmd; // Usa la estructura definida en uart_manager.h

    while (1) {
        // Llama a la función que leerá y parseará el comando de la UART
        uart_read_command(&received_cmd);

        // Envía el color a la cola del LED.
        // received_cmd.color ya debería ser un led_rgb_evento_t
        if (xQueueSend(xQueue_led, &received_cmd.color, portMAX_DELAY) != pdPASS) {
            ESP_LOGE("TaskB", "Error al enviar color a la cola");
        }
        // Usa %lu para uint32_t
       ESP_LOGI("TaskB", "Color enviado: %d (delay: %lu seconds)", received_cmd.color, (unsigned long)received_cmd.delay_seconds); // Usamos (unsigned long) para el especificador de formato como ya discutimos

        vTaskDelay(pdMS_TO_TICKS(received_cmd.delay_seconds * 1000)); // Asumiendo que delay_seconds está en segundos, lo convertimos a milisegundos para pdMS_TO_TICKS
    }
}