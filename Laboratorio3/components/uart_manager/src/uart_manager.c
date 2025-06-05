#include "uart_manager.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "UART_MANAGER";
#define UART_BUF_SIZE 1024
#define UART_PORT UART_NUM_0  // UART0 (USB por defecto en ESP32-S2 Kaluga)

void uart_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_BUF_SIZE, 0, 0, NULL, 0));
    ESP_LOGI(TAG, "UART inicializado en puerto %d", UART_PORT);
}

void uart_read_command(led_rgb_evento_t *color, uint32_t *delay_seconds) {
    uint8_t data[UART_BUF_SIZE];
    char color_str[20];
    int pos = 0;

    // Leer datos crudos del UART
    int len = uart_read_bytes(UART_PORT, data, UART_BUF_SIZE - 1, pdMS_TO_TICKS(100));
    if (len <= 0) {
        *color = LED_EVENT_INVALIDO;
        return;
    }
    data[len] = '\0';  // Null-terminate

    // Parsear comando (formato: "Color Tiempo")
    if (sscanf((char *)data, "%19s %u", color_str, delay_seconds) != 2) {
        ESP_LOGE(TAG, "Formato inválido. Usar: 'Color Tiempo' (ej: 'Rojo 10')");
        *color = LED_EVENT_INVALIDO;
        return;
    }

    // Mapear string a led_rgb_evento_t
    if (strcmp(color_str, "Rojo") == 0) *color = LED_EVENT_ROJO;
    else if (strcmp(color_str, "Verde") == 0) *color = LED_EVENT_VERDE;
    else if (strcmp(color_str, "Azul") == 0) *color = LED_EVENT_AZUL;
    else if (strcmp(color_str, "Blanco") == 0) *color = LED_EVENT_BLANCO;
    else if (strcmp(color_str, "Amarillo") == 0) *color = LED_EVENT_AMARILLO;
    else if (strcmp(color_str, "Cian") == 0) *color = LED_EVENT_CIAN;
    else {
        ESP_LOGE(TAG, "Color no válido: %s", color_str);
        *color = LED_EVENT_INVALIDO;
    }

    ESP_LOGI(TAG, "Comando recibido: Color=%s, Tiempo=%u", color_str, *delay_seconds);
}