#ifndef LED_DELAY_H_
#define LED_DELAY_H_

#include <stdint.h> // Asegúrate de tener esta inclusión si no está en otro lado

// Define las funciones de delay
void delay_ms(uint32_t ms);
void delay_s(uint32_t s);
void delay_ciclos(uint32_t ciclos); // Si decides mantenerla

#endif // LED_DELAY_H_