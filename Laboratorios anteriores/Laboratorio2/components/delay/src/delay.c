#include <stdio.h>
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "rtc_wdt.h"
#include "hal/wdt_types.h"
#include "delay.h"

static const char *TAG = "DELAYS";

//FUNCIONES PARA EL WATCHDOG
void watchdog_inicializar(void)
{
    ESP_LOGI(TAG, "Inicializando watchdog por hardware (RWDT)");

    rtc_wdt_protect_off();  // Desbloquear registros de configuración

    rtc_wdt_disable();      // Asegurar que está deshabilitado antes de configurar

    // Configurar acción del Stage 0 (único stage requerido)
    rtc_wdt_set_stage(RTC_WDT_STAGE0, RTC_WDT_STAGE_ACTION_RESET_SYSTEM);

    // Configurar el tiempo antes de reset: 1000000 ticks de reloj lento (~5 s)
    rtc_wdt_set_time(RTC_WDT_STAGE0, 5000000);

    // Habilitar el watchdog
    rtc_wdt_enable();

    rtc_wdt_protect_on();   // Volver a proteger los registros

    ESP_LOGI(TAG, "Watchdog habilitado. Sistema se reiniciará si no se resetea en ~5 s.");
}

void watchdog_resetear(void)
{
    rtc_wdt_protect_off();
    rtc_wdt_feed();          // Con este reseteamos
    rtc_wdt_protect_on();
}

// FUNCIONES PARA EL DELAY
void delay_ms(uint32_t ms)
{
    ESP_LOGI(TAG, "Delay de: %u ms", (unsigned int)ms);
    esp_rom_delay_us(ms * 1000); // Convertir ms a us
}

void delay_s(uint32_t s)
{ // Resetea el WD en una iteración para no colgar la maquina si ingresamos un tiempo muy largo
    for (uint32_t i = 0; i < s; i++) {
        esp_rom_delay_us(200000);     // Delay de 200 ms
        watchdog_resetear();          // Alimenta al watchdog
        esp_rom_delay_us(200000);     // Otro delay de 200 ms
        watchdog_resetear();          // Otra vez, para seguridad
        esp_rom_delay_us(200000);     // Total 600 ms por iteración
        watchdog_resetear();
        esp_rom_delay_us(400000);     // Resto del segundo
        watchdog_resetear();
    }
}



