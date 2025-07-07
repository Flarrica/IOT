#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "led_rgb.h"
#include "esp_mac.h"
#include "task_a.h"

static TimerHandle_t blink_timer = NULL;
static bool led_on = false;

void vBlinkTimerCallback(TimerHandle_t xTimer)
{
    if (led_on) {
        led_rgb_set_event(LED_EVENT_APAGAR);
        led_on = false;
    } else {
        led_rgb_set_event(current_color);
        led_on = true;
    }
}


void task_a(void *pvParameters) {

    blink_timer = xTimerCreate("BlinkTimer", pdMS_TO_TICKS(500), pdTRUE, NULL, vBlinkTimerCallback);
    //Chequeamos errores
    if (blink_timer == NULL) {
        printf("Error al crear el timer de parpadeo\n");
        vTaskDelete(NULL);
    }
    //Arranca el timer
    xTimerStart(blink_timer, 0);

    while (1) {
        // Espero notificación de nuevo color desde task_c
        xSemaphoreTake(color_semaphore, portMAX_DELAY);
        // Al llegar nuevo color, reseteo parpadeo
        led_on = false;
        led_rgb_set_event(LED_EVENT_APAGAR);
    }
}