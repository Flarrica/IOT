#ifndef LED_RGB_CONTROL_H_
#define LED_RGB_CONTROL_H_

// Inicializa el LED RGB embebido en la placa
void led_rgb_inicializar(void);

// Enciende el LED en color rojo
void led_rgb_prendo_ROJO(void);

// Enciende el LED en color verde
void led_rgb_prendo_VERDE(void);

// Enciende el LED en color azul
void led_rgb_prendo_AZUL(void);

// Enciende el LED en color blanco
void led_rgb_prendo_BLANCO(void);

// Apaga el LED (RGB = 0,0,0)
void led_rgb_apagar(void);

#endif  // LED_RGB_H_