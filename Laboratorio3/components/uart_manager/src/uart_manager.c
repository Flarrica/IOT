#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h" // ¡IMPORTANTE! Necesario para QueueHandle_t y xQueueCreate
#include <inttypes.h> // Necesario para PRIu32
#include <stdio.h>
#include "uart_manager.h" // Contiene UART_RX_BUF_SIZE y uart_command_t
#include "led_rgb.h"      // Contiene la definición de led_rgb_evento_t

#define UART_PORT UART_NUM_0

// ¡¡AÑADIR ESTA LÍNEA AQUÍ!!
// Definición de la cola. Debe ser una variable global.
QueueHandle_t xQueue_led;

// Inicializa la UART
void uart_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // Instala el driver de UART
    uart_driver_install(UART_PORT, UART_RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    // Configura los parámetros de la UART
    uart_param_config(UART_PORT, &uart_config);
    // Configura los pines TX y RX. Asegúrate de que estos GPIOs sean correctos para tu ESP32S2.
    uart_set_pin(UART_PORT, 1, 2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    ESP_LOGI("UART_MGR", "UART Inicializado en GPIO1 (TX), GPIO2 (RX)");

    // ¡¡AÑADIR ESTAS LÍNEAS AQUÍ PARA CREAR LA COLA!!
    // Crear la cola FreeRTOS
    // El tamaño de la cola (por ejemplo, 10) y el tamaño de cada elemento (sizeof(led_rgb_evento_t))
    // deben ser apropiados para lo que vas a enviar.
    xQueue_led = xQueueCreate(10, sizeof(led_rgb_evento_t));
    if (xQueue_led == NULL) {
        ESP_LOGE("UART_MGR", "Fallo al crear la cola xQueue_led");
        // Puedes añadir una lógica de manejo de errores aquí, como un bucle infinito
        // para evitar que el programa continúe sin la cola.
        while(1) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    } else {
        ESP_LOGI("UART_MGR", "Cola xQueue_led creada exitosamente");
    }
}

// Envía una cadena de texto por UART
void uart_send_string(const char *str) {
    uart_write_bytes(UART_PORT, str, strlen(str));
}

// Lee un comando UART y lo parsea en una estructura uart_command_t
void uart_read_command(uart_command_t *cmd) {
    uint8_t data[UART_RX_BUF_SIZE];
    int len = uart_read_bytes(UART_PORT, data, (UART_RX_BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);

    if (len > 0) {
        data[len] = '\0';
        char color_str[10];
        uint32_t delay_val;

        if (sscanf((char *)data, "%9[^,],%lu", color_str, &delay_val) == 2) {
            if (strcmp(color_str, "Rojo") == 0) {
                cmd->color = LED_EVENT_ROJO;
            } else if (strcmp(color_str, "Verde") == 0) {
                cmd->color = LED_EVENT_VERDE;
            } else if (strcmp(color_str, "Azul") == 0) {
                cmd->color = LED_EVENT_AZUL;
            } else if (strcmp(color_str, "Apagar") == 0) {
                cmd->color = LED_EVENT_APAGAR;
            } else {
                ESP_LOGW("UART_MGR", "Color desconocido recibido: %s", color_str);
                cmd->color = LED_EVENT_APAGAR;
            }
            cmd->delay_seconds = delay_val;

            ESP_LOGI("UART_MGR", "Comando Parsed: Color %s (%" PRIu32 "), Delay %" PRIu32 " seconds",
         color_str, (unsigned long)cmd->color, (unsigned long)cmd->delay_seconds);
        }
         else {
            ESP_LOGW("UART_MGR", "Formato de comando UART inválido: %s", (char*)data);
            cmd->color = LED_EVENT_APAGAR;
            cmd->delay_seconds = 1;
        }
    } else {
        cmd->color = LED_EVENT_APAGAR;
        cmd->delay_seconds = 1;
    }
}