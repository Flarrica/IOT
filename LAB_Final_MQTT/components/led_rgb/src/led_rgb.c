#include <stdio.h>
#include <strings.h>

#include "esp_log.h"
#include "led_rgb.h"
#include "led_strip.h"
#include "driver/rmt.h"
#include "driver/gpio.h"

static const char *TAG = "LED_RGB";
static led_strip_t *strip = NULL;

void led_rgb_inicializar(void)
{
    ESP_LOGI(TAG, "Inicializando LED RGB...");
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(KALUGA_RGB_LED_PIN, RMT_CHANNEL_0);
    
    config.clk_div = 2;  // Para WS2812

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(1, (led_strip_dev_t)config.channel);
    strip = led_strip_new_rmt_ws2812(&strip_config);
}

void led_rgb_set_event(led_rgb_evento_t evento)
{
    if (!strip) return;

    switch (evento) {
        case LED_EVENT_ROJO:
            strip->set_pixel(strip, 0, 255, 0, 0);
            break;
        case LED_EVENT_VERDE:
            strip->set_pixel(strip, 0, 0, 255, 0);
            break;
        case LED_EVENT_AZUL:
            strip->set_pixel(strip, 0, 0, 0, 255);
            break;
        case LED_EVENT_BLANCO:
            strip->set_pixel(strip, 0, 255, 255, 255);
            break;
        case LED_EVENT_AMARILLO:
            strip->set_pixel(strip, 0, 255, 255, 0);
            break;
        case LED_EVENT_CIAN:
            strip->set_pixel(strip, 0, 0, 255, 255);
            break;
        case LED_EVENT_APAGAR:
        default:
            strip->set_pixel(strip, 0, 0, 0, 0);
            break;
    }

    strip->refresh(strip, 100);  // Asegura que el cambio se vea
}

led_rgb_evento_t led_rgb_string_to_color(const char *color_str) {
    if (strcasecmp(color_str, "rojo") == 0) {
        return LED_EVENT_ROJO;
    } else if (strcasecmp(color_str, "verde") == 0) {
        return LED_EVENT_VERDE;
    } else if (strcasecmp(color_str, "azul") == 0) {
        return LED_EVENT_AZUL;
    } else if (strcasecmp(color_str, "amarillo") == 0) {
        return LED_EVENT_AMARILLO;
    } else if (strcasecmp(color_str, "cian") == 0) {
        return LED_EVENT_CIAN;
    } else if (strcasecmp(color_str, "blanco") == 0) {
        return LED_EVENT_BLANCO;
    } else if (strcasecmp(color_str, "apagar") == 0) {
        return LED_EVENT_APAGAR;
    }
    return LED_EVENT_INVALIDO;
}