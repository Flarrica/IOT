#include <stdio.h>
#include "esp_log.h"
#include "led_rgb.h"
#include "led_strip.h"
#include "driver/rmt.h"


#include "driver/gpio.h" // Asegúrate de que esta línea exista
#include "led_rgb.h"
// ... otros includes ...
static const char *TAG = "LED_RGB";
static led_strip_t *strip = NULL;
static led_rgb_evento_t evento_pendiente = LED_EVENT_INVALIDO;

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
    evento_pendiente = evento;
}

void led_rgb_bucle(void)
{
    static led_rgb_evento_t evento_previo = LED_EVENT_INVALIDO;
    ESP_LOGI(TAG, "Entra bucle LED");
    if (!strip) return;
        if (evento_pendiente != LED_EVENT_INVALIDO && evento_pendiente != evento_previo) {
            switch (evento_pendiente) {
                case LED_EVENT_ROJO:
                    strip->set_pixel(strip, 0, 255, 0, 0);
                    ESP_LOGI(TAG, "LED encendido en ROJO");
                    break;
                case LED_EVENT_VERDE:
                    strip->set_pixel(strip, 0, 0, 255, 0);
                    ESP_LOGI(TAG, "LED encendido en VERDE");
                    break;
                case LED_EVENT_AZUL:
                    strip->set_pixel(strip, 0, 0, 0, 255);
                    ESP_LOGI(TAG, "LED encendido en AZUL");
                    break;
                case LED_EVENT_BLANCO:
                    strip->set_pixel(strip, 0, 255, 255, 255);
                    ESP_LOGI(TAG, "LED encendido en BLANCO");
                    break;
                case LED_EVENT_AMARILLO:
                    strip->set_pixel(strip, 0, 255, 255, 0);
                    ESP_LOGI(TAG, "LED encendido en AMARILLO");
                    break;
                case LED_EVENT_CIAN:
                    strip->set_pixel(strip, 0, 0, 255, 255);
                    ESP_LOGI(TAG, "LED encendido en CIAN");
                    break;
                case LED_EVENT_APAGAR:
                default:
                    strip->set_pixel(strip, 0, 0, 0, 0);
                    ESP_LOGI(TAG, "LED apagado");
                    break;
            }
            strip->refresh(strip, 100);
            evento_previo = evento_pendiente;  // Limpia evento después de aplicarlo
        }
    
}