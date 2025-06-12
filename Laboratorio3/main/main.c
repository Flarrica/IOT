#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "task_c.h"
#include "task_b.h"
#include "task_a.h"
#include "led_rgb.h"
#include "shared_lib.h"

#include "esp_log.h"

void app_main(void) {
    // Inicialización de logs, UART, LED y tareas
    //esp_log_level_set("TASK_C", ESP_LOG_INFO);
    esp_log_level_set("ws2812", ESP_LOG_WARN);
    
    led_rgb_inicializar();   // Inicializa el LED
    inicializar_recursos_globales();  // Inicializamos los recursos globales

    xTaskCreate(task_a, "task_a", 2048, NULL, 6, NULL); 
    xTaskCreate(task_b, "task_b", 4096, NULL, 10, NULL);
    xTaskCreate(task_c, "task_c", 4096, NULL, 8, NULL);
    
}