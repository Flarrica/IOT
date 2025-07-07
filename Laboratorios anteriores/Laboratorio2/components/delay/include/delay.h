#ifndef DELAY_H_
#define DELAY_H_

#include <stdint.h> // Asegúrate de tener esta inclusión si no está en otro lado

// Define las funciones de delay
void delay_ms(uint32_t ms);
void delay_s(uint32_t s);
void delay_ciclos(uint32_t ciclos); // Si decides mantenerla
void watchdog_inicializar(void); // Inicializa watchdog
void watchdog_resetear(void); // Resetea watchdog en bucle de main
#endif // DELAY_H_