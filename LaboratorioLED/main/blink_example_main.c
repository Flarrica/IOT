#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "color_cycle";

#define BLINK_GPIO CONFIG_BLINK_GPIO

#ifdef CONFIG_BLINK_LED_STRIP

static led_strip_handle_t led_strip;

static const uint8_t colors[][3] = {
    {0, 255, 0},     // Verde
    {0, 0, 255},     // Azul
    {200, 0, 0},    // Naranja (rojo bajo + verde)
    {250, 250, 250}  // Blanco
};

static void set_color(uint8_t r, uint8_t g, uint8_t b)
{
    led_strip_set_pixel(led_strip, 0, r, g, b);
    led_strip_refresh(led_strip);
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Configurando LED RGB con colores visibles");

    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1,
    };

#if CONFIG_BLINK_LED_STRIP_BACKEND_RMT
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10 MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
#elif CONFIG_BLINK_LED_STRIP_BACKEND_SPI
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
#else
#error "unsupported LED strip backend"
#endif

    led_strip_clear(led_strip);
}

#elif CONFIG_BLINK_LED_GPIO

static void configure_led(void)
{
    ESP_LOGI(TAG, "Configurando LED GPIO");
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

#else
#error "unsupported LED type"
#endif

void app_main(void)
{
    configure_led();

    int color_index = 0;
    int total_colors = sizeof(colors) / sizeof(colors[0]);

    while (1) {
        uint8_t r = colors[color_index][0];
        uint8_t g = colors[color_index][1];
        uint8_t b = colors[color_index][2];

        ESP_LOGI(TAG, "Mostrando color RGB(%d, %d, %d)", r, g, b);
        set_color(r, g, b);

        color_index = (color_index + 1) % total_colors;
        vTaskDelay(pdMS_TO_TICKS(1500)); // Espera 1 segundo
    }
}
