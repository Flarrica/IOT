#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "uart_manager.h"
#include "task_b.h"
#include "task_c.h"
#include "led_rgb.h"

SemaphoreHandle_t color_semaphore;

void app_main(void) {
    // Inicialización de UART y LED
    uart_init();
    led_rgb_inicializar();

    // Inicializar semáforo binario para acceso al LED
    color_semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(color_semaphore); // Dar el semáforo inicialmente

    // Crear tareas
    xTaskCreate(task_b, "task_b", 4096, NULL, 10, NULL);
    xTaskCreate(task_c, "task_c", 4096, NULL, 9, NULL);
}