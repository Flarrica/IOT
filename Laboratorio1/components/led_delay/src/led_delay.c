#include <stdio.h>
#include "esp_log.h"
#include "led_delay.h"

static const char *TAG = "LED_DELAY";

// Ayudita para formato de int
//Tipo	    Tamaño	 Rango de valores	Definición
//uint8_t	1 byte	 0 a 255	        unsigned
//uint16_t	2 bytes	 0 a 65,535	        unsigned
//int32_t	4 bytes	 -2.1e9 a 2.1e9	    signed

// Acá ponemos nuestro candidato para cantidad de ciclos para un milisegundo
#define CICLOS_POR_MS 250000  // Valor estimado

volatile uint8_t auxiliar = 0;

void delay_ciclos(uint8_t ciclos)
{
    for (uint8_t i = 0; i < ciclos; i++) {
        auxiliar += 0;  // Consumir tiempo sin hacer nada útil
    }
}

void delay_ms(uint32_t ms)
{
    ESP_LOGI(TAG, "Delay de %u ms", ms);
    delay_ciclos(ms * CICLOS_POR_MS);
}

void delay_s(uint32_t s)
{
    ESP_LOGI(TAG, "Delay de %u s", s);
    delay_ms(s * 1000);
}