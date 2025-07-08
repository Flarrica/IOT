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
#include "nvs_flash.h"
#include "nvs.h"
#include "inttypes.h"
#include "audio_player.h"
#include "esp_sntp.h"
#include "logger.h"
#include "esp_vfs.h"
#include "dirent.h" 


static const char *TAG = "task_mqtt";
static esp_mqtt_client_handle_t client = NULL;
static bool mqtt_suscripto = false;

#define NVS_NAMESPACE "mqtt_config"
#define NVS_KEY_BROKER "broker_url"

reproductor_estado_t estado_reproductor = {"detenido", 50, "Ninguna"};

static void log_error_if_nonzero(const char *message, int error_code) {
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}


void publicar_lista_de_pistas_mqtt(void) {
    DIR *dir = opendir("/spiffs");
    if (!dir) {
        ESP_LOGE("MQTT_MUSICA", "No se pudo abrir /spiffs");
        return;
    }

    struct dirent *entry;
    char json[512] = "[";
    bool first = true;

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".wav")) {
            if (!first) strcat(json, ",");
            first = false;
            strcat(json, "\"");
            strcat(json, entry->d_name);
            strcat(json, "\"");
        }
    }
    strcat(json, "]");
    closedir(dir);

    esp_mqtt_client_publish(mqtt_get_client(), TOPIC_MUSICA, json, 0, 1, 0);
    ESP_LOGI("MQTT_MUSICA", "Pistas publicadas: %s", json);
}

esp_err_t mqtt_guardar_url(const char *url) {
    ESP_LOGI(TAG, "Guardando MQTT URL en NVS: %s", url);
    nvs_handle_t nvs;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open() falló: %s", esp_err_to_name(err));
        return err;
    }

    err = nvs_set_str(nvs, NVS_KEY_BROKER, url);
    if (err == ESP_OK) err = nvs_commit(nvs);
    if (err != ESP_OK) ESP_LOGE(TAG, "nvs_commit() falló: %s", esp_err_to_name(err));
    nvs_close(nvs);
    return err;
}

esp_err_t mqtt_leer_url(char *dest, size_t max_len) {
    if (!dest || max_len == 0) return ESP_ERR_INVALID_ARG;
    nvs_handle_t nvs;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs);
    if (err != ESP_OK) return err;
    size_t len = max_len;
    err = nvs_get_str(nvs, NVS_KEY_BROKER, dest, &len);
    nvs_close(nvs);
    return err;
}

void publicar_estado_reproductor(esp_mqtt_client_handle_t client, reproductor_estado_t estado) {
    audio_state_t estado_actual = audio_player_get_state();
    const char *estado_str = audio_state_to_str(estado_actual);

    int volumen_actual = audio_player_get_volume();
    const char *cancion_actual = audio_player_get_song();

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "estado", estado_str);
    cJSON_AddNumberToObject(root, "volumen", volumen_actual);
    cJSON_AddStringToObject(root, "cancion", cancion_actual ? cancion_actual : "Desconocida");

    char *json_string = cJSON_Print(root);
    esp_mqtt_client_publish(client, TOPIC_ESTADO, json_string, 0, 1, 0);
    ESP_LOGI(TAG, "Estado publicado: %s", json_string);

    cJSON_Delete(root);
    free(json_string);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    client = event->client;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT conectado");
            if (!mqtt_suscripto) {
                esp_mqtt_client_subscribe(client, TOPIC_CONTROL, 1);
                esp_mqtt_client_subscribe(client, TOPIC_ESTADO, 1);
                esp_mqtt_client_subscribe(client, TOPIC_LOG, 1);
                esp_mqtt_client_subscribe(client, TOPIC_GET, 1);
                esp_mqtt_client_subscribe(client, TOPIC_MUSICA, 1);

                mqtt_suscripto = true;
            }

            esp_mqtt_client_publish(client, TOPIC_ENERGIA, crear_evento_energia(), 0, 1, 0);
            publicar_estado_reproductor(client, estado_reproductor);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT desconectado");
            mqtt_suscripto = false;
            break;

        case MQTT_EVENT_DATA: {
            char topic[event->topic_len + 1];
            char payload[event->data_len + 1];
            memcpy(topic, event->topic, event->topic_len);
            memcpy(payload, event->data, event->data_len);
            topic[event->topic_len] = '\0';
            payload[event->data_len] = '\0';

            ESP_LOGI(TAG, "TOPIC: %s", topic);
            ESP_LOGI(TAG, "MSG  : %s", payload);

            if (strcmp(topic, TOPIC_CONTROL) == 0) {
                if      (strcmp(payload, "play") == 0)       audio_player_send_cmd(CMD_PLAY);
                else if (strcmp(payload, "pause") == 0)      audio_player_send_cmd(CMD_PAUSE);
                else if (strcmp(payload, "stop") == 0)       audio_player_send_cmd(CMD_STOP);
                else if (strcmp(payload, "next") == 0)       audio_player_send_cmd(CMD_NEXT);
                else if (strcmp(payload, "previous") == 0)   audio_player_send_cmd(CMD_PREV);
                else if (strcmp(payload, "volume_up") == 0)  audio_player_send_cmd(CMD_VOL_UP);
                else if (strcmp(payload, "volume_down") == 0)audio_player_send_cmd(CMD_VOL_DOWN);
                else ESP_LOGW(TAG, "Comando desconocido: %s", payload);

                publicar_estado_reproductor(client, estado_reproductor);
            }
            else if (strcmp(topic, TOPIC_GET) == 0) {
                ESP_LOGI(TAG, "Solicitud de logger recibida por TOPIC_GET");
                logger_publicar_todo();
            }
            else if (event->topic && strcmp(event->topic, "/placaKaluga/mejorGrupo/nintendo/musica") == 0) {
                if (strncmp(event->data, "listar", event->data_len) == 0) {
                    publicar_lista_de_pistas_mqtt();
                }
                else if (strncmp(payload, "borrar,", 7) == 0) {
                    const char *filename = payload + 7;
                    char path[64];
                    snprintf(path, sizeof(path), "/spiffs/%s", filename);

                    if (unlink(path) == 0) {
                        ESP_LOGI("MQTT_MUSICA", "Archivo borrado por MQTT: %s", filename);
                    } else {
                        ESP_LOGE("MQTT_MUSICA", "Error al borrar archivo: %s", filename);
                    }
                }
            }
            break;
        }

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle) {
                log_error_if_nonzero("esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("socket errno", event->error_handle->esp_transport_sock_errno);
            }
            break;

        default:
            break;
    }
}

esp_err_t task_mqtt_start(void *handler_args) {
    char mqtt_url[128] = "mqtt://broker.hivemq.com";
    mqtt_leer_url(mqtt_url, sizeof(mqtt_url));

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = mqtt_url,
        .session.protocol_ver = MQTT_PROTOCOL_V_5,
        .session.last_will.topic = "/topic/will",
        .session.last_will.msg = "i will leave",
        .session.last_will.msg_len = 12,
        .session.last_will.qos = 1,
        .session.last_will.retain = true,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    if (!client) return ESP_FAIL;

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, handler_args);
    esp_mqtt_client_start(client);
    return ESP_OK;
}

char* crear_evento_energia() {
    static char mensaje[200];

    time_t now = 0;
    time(&now);
    struct tm timeinfo = {0};
    localtime_r(&now, &timeinfo);

    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", &timeinfo);

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "evento", "reabastecimiento");
    cJSON_AddStringToObject(root, "ts", timestamp);

    snprintf(mensaje, sizeof(mensaje), "%s", cJSON_PrintUnformatted(root));
    cJSON_Delete(root);

    return mensaje;
}

esp_mqtt_client_handle_t mqtt_get_client(void) {
    return client;
}