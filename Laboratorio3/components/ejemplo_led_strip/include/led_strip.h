#ifndef _LED_STRIP_H_
#define _LED_STRIP_H_

#include "esp_err.h" // Para esp_err_t
#include "driver/gpio.h" // Para GPIO_NUM_X (KALUGA_RGB_LED_PIN)
#include "driver/rmt.h" // Para rmt_channel_t, rmt_config_t

// Declaración adelantada de la estructura interna
// Esto es importante para que el compilador sepa que 'led_strip_t' existe.
typedef struct led_strip_t led_strip_t;

/**
 * @brief Estructura para almacenar los punteros a funciones de la tira de LED.
 * Esta es la interfaz abstracta para cualquier controlador de tira de LED.
 */
struct led_strip_t {
    esp_err_t (*set_pixel)(led_strip_t *strip, uint32_t index, uint32_t red, uint32_t green, uint32_t blue);
    esp_err_t (*refresh)(led_strip_t *strip, uint32_t timeout_ms);
    esp_err_t (*clear)(led_strip_t *strip, uint32_t timeout_ms);
    esp_err_t (*del)(led_strip_t *strip);
};

/**
 * @brief Configuración por defecto para la tira de LED
 * @param max_leds Número máximo de LEDs en la tira
 * @param dev Número del canal RMT
 */
#define LED_STRIP_DEFAULT_CONFIG(max_leds, dev) { \
    .max_leds = max_leds, \
    .dev = dev, \
}

/**
 * @brief Estructura de configuración de la tira de LED
 */
typedef struct {
    uint32_t max_leds;      /*!< Número máximo de LEDs */
    void *dev;              /*!< Handle específico del dispositivo, por ejemplo, canal RMT */
} led_strip_config_t;

/**
 * @brief Crea un nuevo controlador RMT para tira de LED WS2812
 * @param config Configuración de la tira de LED
 * @return Handle de la tira de LED
 */
led_strip_t *led_strip_new_rmt_ws2812(const led_strip_config_t *config);

/**
 * @brief Inicializa el LED RGB embebido (WS2812)
 * @param strip Puntero al puntero del handle de la tira de LED
 * @return ESP_OK en caso de éxito, o un código de error si falla
 */
esp_err_t led_rgb_init(led_strip_t **strip); // Esta es la función que te causaba error de declaración implícita

// Definiciones de PIN y número de LEDs, si son generales para la librería
#define KALUGA_RGB_LED_PIN GPIO_NUM_45 // Asegúrate de que este sea el GPIO correcto
#define KALUGA_RGB_LED_NUMBER 1

#endif // _LED_STRIP_H_