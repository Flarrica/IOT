#ifndef LED_DELAY_H_
#define LED_DELAY_H_

#include <stdint.h>  // Tipos estándar de tamaño fijo

// Ejecuta un bucle que no hace nada para consumir ciclos de CPU.
// Se usa como base para crear el resto de retardos aproximados.
void delay_ciclos(uint32_t ciclos);

// Retardo aproximado en milisegundos (ms).
// Usa una cantidad calibrada de ciclos por milisegundo.
void delay_ms(uint32_t ms);

// Retardo aproximado en segundos (s).
// Internamente usa delay_ms().
void delay_s(uint32_t s);

#endif  // LED_DELAY_H_