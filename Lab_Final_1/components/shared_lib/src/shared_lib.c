#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "shared_lib.h"
#include "led_rgb.h"
#include "esp_log.h"

// Definición de las variables globales
SemaphoreHandle_t color_semaphore = NULL;
SemaphoreHandle_t timer_done_semaphore = NULL;
led_rgb_evento_t current_color = LED_EVENT_APAGAR;
SemaphoreHandle_t i2c_mutex = NULL;
QueueHandle_t io_mutex = NULL;
QueueHandle_t color_queue = NULL;

void inicializar_recursos_globales(void) {
    color_semaphore = xSemaphoreCreateBinary();
    timer_done_semaphore = xSemaphoreCreateBinary();
    color_queue = xQueueCreate(50, sizeof(color_event_t));

    // Chequear que todo este inicializado y sino, dar ERROR
    if (color_semaphore == NULL || timer_done_semaphore == NULL || color_queue == NULL) {
        ESP_LOGE("SHARED", "Error creando recursos globales");
        abort();
    }
}

