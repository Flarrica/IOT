#include "task_mqtt.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include "inttypes.h"
#include "audio_player.h"

static const char *TAG = "task_mqtt";
static esp_mqtt_client_handle_t client = NULL;

// Variable global para el estado del reproductor
reproductor_estado_t estado_reproductor = {"detenido", 50, "Ninguna"};

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

// Función para publicar el estado del reproductor
void publicar_estado_reproductor(esp_mqtt_client_handle_t client, reproductor_estado_t estado) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "estado", estado.estado);
    cJSON_AddNumberToObject(root, "volumen", estado.volumen);
    cJSON_AddStringToObject(root, "cancion", estado.cancion);

    char *json_string = cJSON_Print(root);

    esp_mqtt_client_publish(client, TOPIC_ESTADO, json_string, 0, 1, 0);
    ESP_LOGI(TAG, "Published estado: %s", json_string);

    cJSON_Delete(root);
    free(json_string);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    ESP_LOGD(TAG, "free heap size is %" PRIu32 ", minimum %" PRIu32, esp_get_free_heap_size(), esp_get_minimum_free_heap_size());
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

        //esp_mqtt5_client_set_user_property(&subscribe_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
        //esp_mqtt5_client_set_subscribe_property(client, &subscribe_property);
        msg_id = esp_mqtt_client_subscribe(client, TOPIC_CONTROL, 0);
       // esp_mqtt5_client_delete_user_property(subscribe_property.user_property);
        //subscribe_property.user_property = NULL;
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        // Enviar el estado inicial del reproductor
        //publicar_estado_reproductor(client, estado_reproductor);

        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        //esp_mqtt5_client_set_publish_property(client, &publish_property);
        //msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        //ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        ESP_LOGI(TAG, "TOPIC=%.*s", event->topic_len, event->topic);
        ESP_LOGI(TAG, "DATA=%.*s", event->data_len, event->data);
//Comienza la nueva sección para el control de la musica.

         if (strncmp(event->topic, TOPIC_CONTROL, event->topic_len) == 0) {
            ESP_LOGI(TAG, "Received command: %.*s", event->data_len, event->data);
            if (strncmp(event->data, "play", event->data_len) == 0) {
                ESP_LOGI(TAG, "Received 'play' command");
                // Iniciar la reproducción de música
                audio_player_send_cmd(CMD_PLAY);
               // strcpy(estado_reproductor.estado, "reproduciendo");
              //  publicar_estado_reproductor(client, estado_reproductor);
            } else if (strncmp(event->data, "pause", event->data_len) == 0) {
                ESP_LOGI(TAG, "Received 'pause' command");
                // Pausar la reproducción de música
                audio_player_send_cmd(CMD_STOP);
               // strcpy(estado_reproductor.estado, "pausado");
               // publicar_estado_reproductor(client, estado_reproductor);
            } else if (strncmp(event->data, "next", event->data_len) == 0) {
                ESP_LOGI(TAG, "Received 'next' command");
                // Ir a la siguiente canción
                audio_player_send_cmd(CMD_NEXT);
              //  publicar_estado_reproductor(client, estado_reproductor);
            } else if (strncmp(event->data, "previous", event->data_len) == 0) {
                ESP_LOGI(TAG, "Received 'previous' command");
                // Ir a la canción anterior
                audio_player_send_cmd(CMD_PREV);
              //  publicar_estado_reproductor(client, estado_reproductor);
            } else if (strncmp(event->data, "volume_up", event->data_len) == 0) {
                ESP_LOGI(TAG, "Received 'volume_up' command");
                // Aumentar el volumen
                audio_player_send_cmd(CMD_VOL_UP);
              //  estado_reproductor.volumen += 10;
              //    if (estado_reproductor.volumen > 100) {
               //       estado_reproductor.volumen = 100;
               //    }
              //  publicar_estado_reproductor(client, estado_reproductor);
            } else if (strncmp(event->data, "volume_down", event->data_len) == 0) {
                ESP_LOGI(TAG, "Received 'volume_down' command");
                // Disminuir el volumen
                audio_player_send_cmd(CMD_VOL_DOWN);
             //   estado_reproductor.volumen -= 10;
             //      if (estado_reproductor.volumen < 0) {
             //          estado_reproductor.volumen = 0;
              //       }
             //   publicar_estado_reproductor(client, estado_reproductor);
            } else {
                ESP_LOGW(TAG, "Unknown command received: %.*s", event->data_len, event->data);
            }
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        //print_user_property(event->property->user_property); //Eliminar
        ESP_LOGI(TAG, "MQTT5 return code is %d", event->error_handle->connect_return_code);
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%"PRId32, event_id);
        break;
    }
}

esp_err_t task_mqtt_start(void *handler_args)
{
    esp_mqtt_client_config_t mqtt5_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
        .session.protocol_ver = MQTT_PROTOCOL_V_5,
        .network.disable_auto_reconnect = true,
        //.credentials.username = "123",
        //.credentials.authentication.password = "456",
        .session.last_will.topic = "/topic/will",
        .session.last_will.msg = "i will leave",
        .session.last_will.msg_len = 12,
        .session.last_will.qos = 1,
        .session.last_will.retain = true,
    };

    client = esp_mqtt_client_init(&mqtt5_cfg);
    ESP_LOGI(TAG, "task_mqtt_start: Registering the event = %p", mqtt_event_handler);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, handler_args);
    esp_mqtt_client_start(client);
    return ESP_OK;
}

esp_err_t task_mqtt_publish_estado_reproductor(esp_mqtt_client_handle_t client, reproductor_estado_t estado) {
     publicar_estado_reproductor(client, estado);
    return ESP_OK;
}
