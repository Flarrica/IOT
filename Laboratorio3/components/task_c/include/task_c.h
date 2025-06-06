#ifndef TASK_C_H_ // O un nombre similar y ÚNICO para este archivo
#define TASK_C_H_

#include "freertos/semphr.h" // Necesario para SemaphoreHandle_t
#include "freertos/queue.h"  // Necesario para QueueHandle_t (si se declara una cola aquí)
#include "led_rgb.h"         // Si task_c.h necesita saber sobre led_rgb
#include "uart_manager.h"    // Si task_c.h necesita saber sobre uart_manager (para uart_command_t o la cola)

// Declaraciones de variables globales (extern)
extern SemaphoreHandle_t color_semaphore;
// Si command_queue también se declara aquí, asegúrate de que uart_manager.h no la declare también si es una definición
// Idealmente, command_queue debería declararse extern en uart_manager.h y definirse en main.c o uart_manager.c
// Si la declaraste en uart_manager.h y la incluyes, no la declares aquí.
// Si la declaraste aquí, ¡elimínala de uart_manager.h! La mejor práctica es que el tipo y la cola estén juntos.

// Declaraciones de funciones
void task_c(void *pvParameter);
// Agrega cualquier otra función que `task_c.c` exporte y que otras tareas puedan llamar

#endif /* TASK_C_H_ */ // ¡Este es el #endif que falta!