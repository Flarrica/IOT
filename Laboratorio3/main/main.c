#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "led_rgb.h"        // Asegúrate de que este archivo defina led_rgb_evento_t
#include "uart_manager.h"   // Asegúrate de que este archivo defina uart_command_t y declare extern QueueHandle_t command_queue
#include "task_a.h"
#include "task_b.h"
#include "task_c.h"         // Este ya debería incluir uart_manager.h y declarar extern SemaphoreHandle_t color_semaphore
#include <stdio.h>

#include "freertos/task.h"

#include "driver/uart.h" // Asegúrate de que esta inclusión sea necesaria y correcta
#include "uart_manager.h" // Si tienes task_c, asegúrate de que también esté incluida y declarada
#include "led_RGB.h"
SemaphoreHandle_t color_semaphore;
QueueHandle_t command_queue;
led_rgb_evento_t current_color = LED_EVENT_APAGAR; // Si esta variable se comparte, su acceso debe ser protegido por color_semaphore

void app_main() {
    // Inicializar hardware
    led_rgb_inicializar();
    uart_init(); // Asegúrate de que esta función (definida en uart_manager.c)
                 // inicialice el UART y configure la cola si es necesario.

    // Crear semáforo y cola
    color_semaphore = xSemaphoreCreateMutex();
    if (color_semaphore == NULL) {
        ESP_LOGE("Main", "Error al crear el semáforo color_semaphore");
        // Aquí podrías agregar un bucle infinito o un reinicio para evitar que el programa continúe
        // si un recurso crítico no se puede crear.
        while(1) { vTaskDelay(pdMS_TO_TICKS(100)); }
    }

    command_queue = xQueueCreate(10, sizeof(uart_command_t));
    if (command_queue == NULL) { // ¡Asegúrate de agregar esta verificación!
        ESP_LOGE("Main", "Error al crear la cola command_queue");
        while(1) { vTaskDelay(pdMS_TO_TICKS(100)); }
    }

    // Crear tareas (prioridad: TaskB > TaskA = TaskC)
    // Asegúrate de que los tamaños de stack sean adecuados para cada tarea.
    // 2048 bytes puede ser suficiente para tareas simples, pero tareas más complejas
    // con muchos cálculos o uso intensivo de la pila pueden necesitar más (e.g., 4096 o 8192).
    xTaskCreate(task_a, "TaskA", 2048, (void *)&current_color, 1, NULL); // Pasa la dirección de current_color si task_a la usa
    xTaskCreate(task_b, "TaskB", 2048, NULL, 2, NULL);
    xTaskCreate(task_c, "TaskC", 2048, (void *)&current_color, 1, NULL); // Pasa la dirección de current_color a task_c
}