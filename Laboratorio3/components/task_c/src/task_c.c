
#include "stdlib.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "led_rgb.h" // Esta línea es crucial para led_rgb_evento_t
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h" 
#include "uart_manager.h"
#include "task_c.h" 

void timer_callback(TimerHandle_t xTimer) {
    led_rgb_evento_t *color = (led_rgb_evento_t *)pvTimerGetTimerID(xTimer);
    xSemaphoreTake(color_semaphore, portMAX_DELAY);
    led_rgb_set_event(*color);  // Usa tu función para cambiar el color
    xSemaphoreGive(color_semaphore);
    free(color);  // Liberar memoria
}

void task_c(void *pvParameters) {
    uart_command_t cmd;
    while (1) {
        if (xQueueReceive(command_queue, &cmd, portMAX_DELAY)) {
            led_rgb_evento_t *color_ptr = malloc(sizeof(led_rgb_evento_t));
            *color_ptr = cmd.color;
            TimerHandle_t timer = xTimerCreate(
                "DelayTimer",
                pdMS_TO_TICKS(cmd.delay_seconds * 1000),
                pdFALSE,  // One-shot
                (void *)color_ptr,
                timer_callback
            );
            xTimerStart(timer, 0);
        }
    }
}