#ifndef SHARED_LIB_H_
#define SHARED_LIB_H_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "led_rgb.h"

// Variables compartidas
extern SemaphoreHandle_t color_semaphore;
extern SemaphoreHandle_t timer_done_semaphore;
extern led_rgb_evento_t current_color;

extern QueueHandle_t command_queue;

typedef struct {
    led_rgb_evento_t color;
    uint32_t delay_seconds;
} color_event_t;

// Función de inicialización
void inicializar_recursos_globales(void);

#endif // SHARED_LIB_H_