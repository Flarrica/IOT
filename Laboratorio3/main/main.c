#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "led_rgb.h"
#include "uart_manager.h"
#include "task_a.h"
#include "task_b.h"
#include "task_c.h"

SemaphoreHandle_t color_semaphore;
QueueHandle_t command_queue;
led_rgb_evento_t current_color = LED_EVENT_APAGAR;

void app_main() {
    // Inicializar hardware
    led_rgb_inicializar();
    uart_init();

    // Crear semáforo y cola
    color_semaphore = xSemaphoreCreateMutex();
    command_queue = xQueueCreate(10, sizeof(uart_command_t));

    // Crear tareas (prioridad: TaskB > TaskA = TaskC)
    xTaskCreate(task_a, "TaskA", 2048, NULL, 1, NULL);
    xTaskCreate(task_b, "TaskB", 2048, NULL, 2, NULL);
    xTaskCreate(task_c, "TaskC", 2048, NULL, 1, NULL);
}