#include "logger.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <string.h>
#include <stdio.h>
#include "task_mqtt.h"
#include "esp_log.h"
#include "cJSON.h"


#define LOGGER_NAMESPACE "logger"


static const char *TAG = "LOGGER";

static const char *logger_event_names[] = {
    "PLAY",
    "PAUSE",
    "NEXT",
    "PREVIOUS",
    "STOP"
};

static size_t get_next_index(size_t index) {
    return (index + 1) % LOGGER_EVENT_MAX;
}

// Comparador para qsort
static int comparar_por_timestamp(const void *a, const void *b) {
    const logger_event_t *ea = (const logger_event_t *)a;
    const logger_event_t *eb = (const logger_event_t *)b;
    if (ea->timestamp < eb->timestamp) return -1;
    if (ea->timestamp > eb->timestamp) return 1;
    return 0;
}

esp_err_t logger_init(void) {
    esp_err_t err = nvs_flash_init_partition("logger");

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Logger NVS corrupta o sin espacio. Borrando partición...");
        ESP_ERROR_CHECK(nvs_flash_erase_partition("logger"));
        err = nvs_flash_init_partition("logger");
    }

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error al inicializar partición logger: %s", esp_err_to_name(err));
        return err;
    }

    // Si se inicializó bien, aseguramos que el índice de escritura exista
    nvs_handle_t handle;
    err = nvs_open_from_partition("logger", LOGGER_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "No se pudo abrir namespace logger: %s", esp_err_to_name(err));
        return err;
    }

    uint32_t write_index;
    err = nvs_get_u32(handle, "write_index", &write_index);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // Primer uso, seteo a 0
        write_index = 0;
        ESP_LOGI(TAG, "Inicializando índice de escritura a 0");
        nvs_set_u32(handle, "write_index", write_index);
        nvs_commit(handle);
    }

    nvs_close(handle);
    return ESP_OK;
}

esp_err_t logger_add_event(logger_event_type_t type) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition("logger", LOGGER_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    size_t write_index = 0;
    nvs_get_u32(handle, "write_index", (uint32_t*)&write_index);

    char key[16];
    snprintf(key, sizeof(key), "event_%02d", (int)write_index);

    logger_event_t event = {
        .timestamp = time(NULL),
        .event_type = type
    };

    err = nvs_set_blob(handle, key, &event, sizeof(event));
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    write_index = get_next_index(write_index);
    nvs_set_u32(handle, "write_index", write_index);

    nvs_commit(handle);
    nvs_close(handle);
    return ESP_OK;
}

esp_err_t logger_get_all_events(logger_event_t *events, size_t max_events, size_t *actual_count) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition("logger", LOGGER_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) return err;

    size_t count = 0;

    ESP_LOGI(TAG, "Historial completo del logger (máx %d eventos):", LOGGER_EVENT_MAX);
    for (int i = 0; i < LOGGER_EVENT_MAX; i++) {
        char key[16];
        snprintf(key, sizeof(key), "event_%02d", i);
        size_t len = sizeof(logger_event_t);

        if (nvs_get_blob(handle, key, &events[count], &len) == ESP_OK) {
            ESP_LOGI(TAG, "[%02d] Timestamp: %lld, Tipo: %s", i, (long long)events[count].timestamp,
                (events[count].event_type < sizeof(logger_event_names) / sizeof(logger_event_names[0]))
                    ? logger_event_names[events[count].event_type] : "UNKNOWN");
            count++;
        } else {
            ESP_LOGI(TAG, "[%02d] <vacío>", i);
        }

        if (count >= max_events) break;
    }

    *actual_count = count;
    nvs_close(handle);
    return ESP_OK;
}

void logger_publicar_todo(void) {
    logger_event_t eventos[LOGGER_EVENT_MAX];
    size_t count = 0;

    if (logger_get_all_events(eventos, LOGGER_EVENT_MAX, &count) != ESP_OK) {
        ESP_LOGE("LOGGER", "No se pudo obtener el historial de eventos.");
        return;
    }

    esp_mqtt_client_handle_t client = mqtt_get_client();
    if (client == NULL) {
        ESP_LOGW("LOGGER", "Cliente MQTT no inicializado.");
        return;
    }

    qsort(eventos, count, sizeof(logger_event_t), comparar_por_timestamp);

    cJSON *root = cJSON_CreateArray();

    for (size_t i = 0; i < count; i++) {
        char timestamp_str[32];
        struct tm tm;
        localtime_r(&eventos[i].timestamp, &tm);
        strftime(timestamp_str, sizeof(timestamp_str), "%Y-%m-%d %H:%M:%S", &tm);

        cJSON *evento = cJSON_CreateObject();
        cJSON_AddStringToObject(evento, "ts", timestamp_str);
        cJSON_AddStringToObject(evento, "evento",
            (eventos[i].event_type < sizeof(logger_event_names) / sizeof(logger_event_names[0]))
                ? logger_event_names[eventos[i].event_type] : "UNKNOWN");

        cJSON_AddItemToArray(root, evento);
    }

    char *json_str = cJSON_Print(root);
    esp_mqtt_client_publish(client, TOPIC_LOG, json_str, 0, 1, 0);
    ESP_LOGI("LOGGER", "Logger enviado como JSON: %s", json_str);

    free(json_str);
    cJSON_Delete(root);
}

void logger_publicar_al_arrancar(void) {
    static bool ya_publicado = false;

    if (ya_publicado) return;

    if (mqtt_get_client() != NULL) {
        logger_publicar_todo();
        ya_publicado = true;
    }
}