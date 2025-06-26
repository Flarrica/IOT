/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h" // Necesario para vTaskDelay
#include "freertos/task.h"    // Necesario para vTaskDelay

static const char *TAG = "mqtt_example";

// Define el nuevo tópico único aquí
#define MY_MQTT_TOPIC "/marcos/mi_esp32/mensajes"


static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 * This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        // Publicar en /topic/qos1 como parte del ejemplo, puedes mantenerlo o comentarlo
        msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful to /topic/qos1, msg_id=%d", msg_id);

        // Suscribirse al nuevo tópico que usaremos para la comunicación
        msg_id = esp_mqtt_client_subscribe(client, MY_MQTT_TOPIC, 0);
        ESP_LOGI(TAG, "sent subscribe successful to %s, msg_id=%d", MY_MQTT_TOPIC, msg_id);

        // Suscribirse a /topic/qos1 (parte del ejemplo original)
        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        ESP_LOGI(TAG, "sent subscribe successful to /topic/qos1, msg_id=%d", msg_id);

        // Desuscribirse de /topic/qos1 (parte del ejemplo original)
        msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        ESP_LOGI(TAG, "sent unsubscribe successful from /topic/qos1, msg_id=%d", msg_id);

        // OPCIONAL: Publicar un mensaje desde la ESP32 al tópico para verificar la comunicación en ambas direcciones
        // Descomenta la siguiente línea si quieres que la ESP32 publique un mensaje que verás en tu Mac
        msg_id = esp_mqtt_client_publish(client, MY_MQTT_TOPIC, "Hola desde mi ESP32!", 0, 0, 0);
        ESP_LOGI(TAG, "ESP32 sent its own publish to %s, msg_id=%d", MY_MQTT_TOPIC, msg_id);

        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        // COMENTADO: La ESP32 ya NO publicará "data" automáticamente en el tópico /topic/qos0
        // Esto evita confusión con los mensajes que tú envías.
        // msg_id = esp_mqtt_client_publish(client, MY_MQTT_TOPIC, "data", 0, 0, 0);
        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA - RECEIVED MESSAGE"); // Mensaje de confirmación
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        // No se necesita lógica de filtrado aquí, simplemente imprimirá lo que reciba.
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

// NUEVA FUNCIÓN: Esta tarea se encarga de iniciar el cliente MQTT y mantenerlo activo.
static void mqtt_app_task(void *pvParameters)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL, // Usará la URL definida en menuconfig
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    // Este bucle infinito es CRUCIAL para mantener la tarea de MQTT activa y procesando eventos.
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 1 segundo para no consumir CPU innecesariamente
    }
}


static void mqtt_app_start(void)
{
    // Crea la tarea de FreeRTOS para el cliente MQTT
    // "mqtt_task" es el nombre, 4096 es el tamaño de la pila (bytes), NULL para parámetros, 5 es prioridad.
    xTaskCreate(&mqtt_app_task, "mqtt_task", 4096, NULL, 5, NULL);
}

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    // Configura los niveles de log (puedes ajustarlos si necesitas más/menos detalle)
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("mqtt_example", ESP_LOG_VERBOSE);
    esp_log_level_set("transport_base", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("transport", ESP_LOG_VERBOSE);
    esp_log_level_set("outbox", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());        // Inicializa la memoria NVS para almacenar datos (ej. credenciales Wi-Fi)
    ESP_ERROR_CHECK(esp_netif_init());       // Inicializa la pila de red
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // Crea el bucle de eventos por defecto

    /* Esta función de ayuda configura Wi-Fi o Ethernet, según se seleccione en menuconfig.
     * Lee la sección "Establishing Wi-Fi or Ethernet Connection" en
     * examples/protocols/README.md para más información sobre esta función.
     */
    ESP_ERROR_CHECK(example_connect()); // Conecta la ESP32 a la red Wi-Fi configurada

    mqtt_app_start(); // Inicia la aplicación MQTT creando su tarea dedicada

    // NOTA: No necesitamos un while(1) aquí en app_main() porque
    // la tarea mqtt_app_task ya tiene su propio bucle infinito y mantiene la aplicación activa.
    // app_main() puede retornar una vez que las tareas necesarias han sido creadas.
}
