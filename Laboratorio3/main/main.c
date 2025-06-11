#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "task_c.h"
#include "task_b.h"
#include "task_a.h"
#include "led_rgb.h"
#include "esp_log.h"

void app_main(void) {
    // Inicialización de logs, UART, LED y tareas
    esp_log_level_set("TASK_C", ESP_LOG_INFO);

    led_rgb_inicializar();   // Inicializa el LED
    task_c_init();           // Inicializa el semáforo

    xTaskCreate(task_b, "task_b", 4096, NULL, 10, NULL);  // Tarea que recibe comandos y los pone en la cola
    xTaskCreate(task_c, "task_c", 4096, NULL, 8, NULL);   // Tarea que consume de la cola y lanza parpadeo
    xTaskCreate(task_a, "task_a", 2048, NULL, 6, NULL);   // Tarea opcional que usa `current_color` si aplicara
}