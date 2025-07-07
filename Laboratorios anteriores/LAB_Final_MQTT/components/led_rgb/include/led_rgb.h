#ifndef LED_RGB_H_
#define LED_RGB_H_

typedef enum {
    LED_EVENT_APAGAR = 0,
    LED_EVENT_ROJO,
    LED_EVENT_VERDE,
    LED_EVENT_AZUL,
    LED_EVENT_BLANCO,
    LED_EVENT_AMARILLO,
    LED_EVENT_CIAN,
    LED_EVENT_INVALIDO
} led_rgb_evento_t;

typedef struct {
    led_rgb_evento_t color;
    uint32_t delay_seconds;
} color_event_t;

void led_rgb_inicializar(void);

// Evento de cambio de color
void led_rgb_set_event(led_rgb_evento_t evento);

// Toma string y cambia a evento color. La usa tanto task_b como task_mqtt
led_rgb_evento_t led_rgb_string_to_color(const char *color_str);
#endif  // LED_RGB_H_