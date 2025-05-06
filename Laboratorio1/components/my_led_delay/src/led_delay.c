#include <stdio.h>
#include "esp_log.h"
#include "led_delay.h"



static const char *TAG = "LED_DELAY";

void delay_ms(uint32_t ms)
{
    ESP_LOGI(TAG, "Delay de: %u ms", (unsigned int)ms);
    esp_rom_delay_us(ms * 1000); // Convertir ms a us
}

void delay_s(uint32_t s)
{
    ESP_LOGI(TAG, "Delay de %u s (usando esp_rom_delay_us)", (unsigned int)s);
    esp_rom_delay_us(s * 1000 * 1000); // Convertir s a us
}