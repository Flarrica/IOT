#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_rgb.h"        // Tu propio encabezado
#include "led_strip.h"      // El encabezado de la librería led_strip que me pasaste

static const char *TAG = "led_rgb"; // Etiqueta para ESP_LOGx

// La variable 'led_strip_handle' ahora se manejará externamente,
// ya que 'led_rgb_init' en 'led_strip.c' ya crea y gestiona la tira.
// En tu 'led_strip.c' tienes: 'led_strip_t *embedded_led;'
// Por lo tanto, necesitamos declarar 'embedded_led' como extern aquí
extern led_strip_t *embedded_led;


// --- Implementaciones de Funciones ---

void led_rgb_inicializar(void) {
    ESP_LOGI(TAG, "Inicializando LED RGB usando la librería 'led_strip.c'...");

    // Simplemente llamamos a la función de inicialización proporcionada por tu librería led_strip.c
    // Esta función ya se encarga de configurar el RMT y crear la tira de LED.
    esp_err_t ret = led_rgb_init(&embedded_led); // Pasa la dirección de tu puntero global a embedded_led
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fallo al inicializar la tira de LED RGB: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "LED RGB inicializado correctamente.");
    }
}

void led_rgb_set_color(uint8_t red, uint8_t green, uint8_t blue) {
    if (embedded_led != NULL) { // Asegúrate de que 'embedded_led' esté inicializado
        // Establece el color del primer (y único) LED (índice 0)
        ESP_ERROR_CHECK(embedded_led->set_pixel(embedded_led, 0, red, green, blue));
        // Actualiza la tira de LED para aplicar el color
        ESP_ERROR_CHECK(embedded_led->refresh(embedded_led, 100)); // Usamos un timeout de 100ms
    } else {
        ESP_LOGE(TAG, "Tira de LED no inicializada. Llama a led_rgb_inicializar() primero.");
    }
}

void led_rgb_apagar_todo(void) {
    if (embedded_led != NULL) {
        // Establece todos los píxeles en negro (apagado)
        ESP_ERROR_CHECK(embedded_led->clear(embedded_led, 100)); // Usamos un timeout de 100ms
    }
}

void led_rgb_eliminar(void) {
    if (embedded_led != NULL) {
        // Llama a la función de eliminación de la tira de LED.
        // La implementación de 'ws2812_del' en tu 'led_strip.c' libera la memoria.
        embedded_led->del(embedded_led);
        embedded_led = NULL; // Limpia el puntero después de eliminar
    }
    ESP_LOGI(TAG, "Controlador de LED RGB desinicializado.");
}