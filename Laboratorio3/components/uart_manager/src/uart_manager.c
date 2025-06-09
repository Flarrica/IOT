#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <inttypes.h>
#include <stdio.h>
#include "uart_manager.h"
#include "led_rgb.h"

#define UART_PORT UART_NUM_0

QueueHandle_t xQueue_led;
QueueHandle_t command_queue;

void uart_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    uart_driver_install(UART_PORT, UART_RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, 1, 2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    ESP_LOGI("UART_MGR", "UART Inicializado en GPIO1 (TX), GPIO2 (RX)");

    xQueue_led = xQueueCreate(10, sizeof(led_rgb_evento_t));
    if (xQueue_led == NULL) {
        ESP_LOGE("UART_MGR", "Fallo al crear la cola xQueue_led");
        while(1) vTaskDelay(pdMS_TO_TICKS(1000));
    }

    command_queue = xQueueCreate(10, sizeof(uart_command_t));
    if (command_queue == NULL) {
        ESP_LOGE("UART_MGR", "Fallo al crear la cola command_queue");
        while(1) vTaskDelay(pdMS_TO_TICKS(1000));
    } else {
        ESP_LOGI("UART_MGR", "Cola command_queue creada exitosamente");
    }
}

void uart_send_string(const char *str) {
    uart_write_bytes(UART_PORT, str, strlen(str));
}

void uart_read_command(uart_command_t *cmd) {
    uint8_t data[UART_RX_BUF_SIZE];
    int len = uart_read_bytes(UART_PORT, data, (UART_RX_BUF_SIZE - 1), portMAX_DELAY);  // espera hasta recibir datos

    if (len > 0) {
        data[len] = '\0';

        // Eliminar salto de línea al final si existe
        if (data[len - 1] == '\n' || data[len - 1] == '\r') {
            data[len - 1] = '\0';
        }

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
                ESP_LOGW("UART_MGR", "Color desconocido: %s", color_str);
                cmd->color = LED_EVENT_APAGAR;
            }

            cmd->delay_seconds = delay_val;
            ESP_LOGI("UART_MGR", "Comando válido: Color=%s (%lu), Delay=%lu s",
                     color_str, (unsigned long)cmd->color, (unsigned long)cmd->delay_seconds);
        } else {
            ESP_LOGW("UART_MGR", "Formato inválido: %s", (char*)data);
            cmd->color = LED_EVENT_APAGAR;
            cmd->delay_seconds = 1;
        }
    } else {
        cmd->color = LED_EVENT_APAGAR;
        cmd->delay_seconds = 1;
    }
}