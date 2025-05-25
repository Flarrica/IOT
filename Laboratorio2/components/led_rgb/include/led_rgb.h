#ifndef LED_RGB_H_
#define LED_RGB_H_

typedef enum {
    LED_EVENT_APAGAR = 0,
    LED_EVENT_ROJO,
    LED_EVENT_VERDE,
    LED_EVENT_AZUL,
    LED_EVENT_BLANCO,
    LED_EVENT_AMARILLO,
    LED_EVENT_CIAN
} led_rgb_evento_t;

void led_rgb_inicializar(void);

// Evento de cambio de color
void led_rgb_set_event(led_rgb_evento_t evento);

// Bucle de actualización si fuese necesario
void led_rgb_bucle(void);

#endif  // LED_RGB_H_