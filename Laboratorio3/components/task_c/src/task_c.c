#include "task_c.h"

static const char *TAG = "TASK_C";

// Este es el callback de los timers creados
void vTimerCallback(TimerHandle_t xTimer)
{
    current_color = LED_EVENT_APAGAR; // Primero apagamos al finalizar el contador. Si no hay nada en cola, el LED queda apagado.
    xSemaphoreGive(timer_done_semaphore);
    
}

void task_c(void *pvParameters) {
    
    uart_command_t cmd;

    while (1) {
        // Espera el primer comando
        if (xQueueReceive(command_queue, &cmd, portMAX_DELAY) == pdTRUE) {
            
            // Apenas lo leo, actualizo el color para que task_a lo tome
            current_color = cmd.color;

            // Aviso a task_a que hay nuevo color
            xSemaphoreGive(color_semaphore);
            
            // Creamos el timer
            TimerHandle_t first_timer = xTimerCreate(
                "BlinkTimer",
                pdMS_TO_TICKS(cmd.delay_seconds * 1000),
                pdFALSE,  // No autoreload. Sería one-shot.
                NULL, //No pasamos nada en el ID, no es necesario
                vTimerCallback // La funcion de callback es la que definimos arriba
            );

            if (first_timer != NULL) {
                //Arranca el timer
                xTimerStart(first_timer, 0); 
                // Bloquea el inicio de otra lectura de la cola
                xSemaphoreTake(timer_done_semaphore, portMAX_DELAY);               
            } else {
                ESP_LOGE(TAG, "Error al crear el timer");
            }
        } 
    }
}