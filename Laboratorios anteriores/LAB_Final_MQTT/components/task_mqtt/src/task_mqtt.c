#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_event.h"
#include "freertos/queue.h"
#include "mqtt_client.h"
#include "cJSON.h"

#include "led_rgb.h" // Para usar: led_rgb_string_to_color
#include "task_mqtt.h"
#include "shared_lib.h"

#define TAG "TASK_MQTT"

extern QueueHandle_t command_queue;  // declarada en shared_lib.h

static void log_error_if_nonzero(const char *message, int error_code) {
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            esp_mqtt_client_subscribe(client, "/led/control", 1);
            esp_mqtt_client_publish(client, "/led/status", "ESP32 conectado correctamente", 0, 1, 0);
            break;

        case MQTT_EVENT_DATA: {
            ESP_LOGI(TAG, "TOPIC=%.*s", event->topic_len, event->topic);
            ESP_LOGI(TAG, "DATA=%.*s", event->data_len, event->data);

            // Copiar payload y parsear como JSON
            char *payload = strndup(event->data, event->data_len);
            cJSON *root = cJSON_Parse(payload);
            if (!root) {
                ESP_LOGW(TAG, "JSON inválido");
                free(payload);
                break;
            }

            const cJSON *color_json = cJSON_GetObjectItem(root, "color");
            const cJSON *delay_json = cJSON_GetObjectItem(root, "delay");

            if (cJSON_IsString(color_json)) {
                color_event_t evt;
                evt.color = led_rgb_string_to_color(color_json->valuestring);
                evt.delay_seconds = (cJSON_IsNumber(delay_json)) ? delay_json->valueint : 0;

                if (xQueueSend(command_queue, &evt, pdMS_TO_TICKS(100)) != pdPASS) {
                    ESP_LOGW(TAG, "Cola llena, evento descartado");
                } else {
                    ESP_LOGI(TAG, "Comando encolado desde MQTT: %s, %lu seg", color_json->valuestring, evt.delay_seconds);
                }
            } else {
                ESP_LOGW(TAG, "Campo 'color' ausente o inválido");
            }

            cJSON_Delete(root);
            free(payload);
            break;
        }

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
            log_error_if_nonzero("Transport", event->error_handle->esp_transport_sock_errno);
            break;

        default:
            break;
    }
}

void iniciar_task_mqtt(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
        .session.protocol_ver = MQTT_PROTOCOL_V_5,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}
