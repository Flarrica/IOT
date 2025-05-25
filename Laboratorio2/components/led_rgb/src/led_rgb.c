#include <stdio.h>
#include "esp_log.h"
#include "led_rgb.h"
#include "ejemplo_led_strip.h"
#include "driver/rmt.h"

static const char *TAG = "LED_RGB";

// Objeto controlador del LED (tipo strip)
static led_strip_t *strip = NULL;

// Tenemos que inicializar el periferico RMT porque parece que no se hace automaticamente en este entorno (eso es lo que entiendo)
// Que pretendemos?
// Configurar RMT antes de usar la librería led_strip
// Inicializar led_strip
// Escribir como va a parpadear el led

void led_rgb_inicializar(void)
{
    ESP_LOGI(TAG, "Inicializando LED RGB...");

    // Configura RMT en modo transmisión en GPIO 45 (LED embebido)
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(GPIO_NUM_45, RMT_CHANNEL_0);

    /*
    Que significa la funcion RMT_DEFAULT_CONFIG_TX. 
    #define RMT_DEFAULT_CONFIG_TX(gpio, channel) \
    { \
    .rmt_mode = RMT_MODE_TX, \
    .channel = channel, \
    .gpio_num = gpio, \
    .mem_block_num = 1, \
    .clk_div = 80, \
    .tx_config = { \
        .loop_en = false, \
        .carrier_en = false, \
        .idle_output_en = true, \
        .idle_level = RMT_IDLE_LEVEL_LOW, \
        } \
    }
    */
    config.clk_div = 2;  // Clock divisor para señal WS2812 (20 MHz)
    // Aca hay un tema con la resolucion temporal
    // Con el clk_div por defecto, 1 tick demora 1us
    // Precisamos tener una resolucion de 0,7us o 0,35us por ejemplo
    // Entonces, por defecto serian lo mismo (1 tick)
    // Si dividimos: clk_div = 2
    // 700ns -> 28 ticks
    // 350ns -> 14 ticks
    // Tenemos la definicion adecuada para que el led entienda



    //“Before calling this function, you must configure and install the RMT driver for the selected channel.”
    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    // Configuración del LED strip con 1 solo LED
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(1, (led_strip_dev_t)config.channel);
    strip = led_strip_new_rmt_ws2812(&strip_config);
}

void led_rgb_prendo_ROJO(void)
{
    if (strip) {
        strip->set_pixel(strip, 0, 255, 0, 0);  // Rojo: R=255, G=0, B=0
        strip->refresh(strip, 100);
        ESP_LOGI(TAG, "LED encendido en ROJO");
    }
}

void led_rgb_prendo_VERDE(void)
{
    if (strip) {
        strip->set_pixel(strip, 0, 0, 255, 0);  // Verde
        strip->refresh(strip, 100);
        ESP_LOGI(TAG, "LED encendido en VERDE");
    }
}

void led_rgb_prendo_AZUL(void)
{
    if (strip) {
        strip->set_pixel(strip, 0, 0, 0, 255);  // Azul
        strip->refresh(strip, 100);
        ESP_LOGI(TAG, "LED encendido en AZUL");
    }
}

void led_rgb_prendo_BLANCO(void)
{
    if (strip) {
        strip->set_pixel(strip, 0, 255, 255, 255); // Blanco
        strip->refresh(strip, 100);
        ESP_LOGI(TAG, "LED encendido en BLANCO");
    }
}

void led_rgb_apagar(void)
{
    if (strip) {
        strip->set_pixel(strip, 0, 0, 0, 0);  // Apagar
        strip->refresh(strip, 100);
        ESP_LOGI(TAG, "LED apagado");
    }
}