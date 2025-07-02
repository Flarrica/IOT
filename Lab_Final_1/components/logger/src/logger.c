/*

logger_add_event(), 

Qué hace: Guarda un nuevo evento (play/pausa, siguiente, etc.) con la hora actual en la memoria no volátil.
Uso típico: Se llama cada vez que ocurre una acción de reproducción.

logger_get_all(), 

Qué hace: Carga todos los eventos guardados (hasta max) en el array events.
Uso típico: Para enviar los eventos por MQTT una vez que hay conexión.

logger_mark_sent().

Qué hace: Borra los eventos que ya fueron enviados exitosamente.
Uso típico: Se llama después de recibir el ACK del broker MQTT.

*/

#include "logger.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <string.h>
#include <stdio.h>

#define LOGGER_NAMESPACE "logger"
#define LOGGER_EVENT_MAX 20

static const char *TAG = "LOGGER";

static size_t get_next_index(size_t index) {
    return (index + 1) % LOGGER_EVENT_MAX;
}

esp_err_t logger_init(void) {
    return nvs_flash_init_partition("logger");
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
    for (int i = 0; i < LOGGER_EVENT_MAX && count < max_events; i++) {
        char key[16];
        snprintf(key, sizeof(key), "event_%02d", i);
        size_t len = sizeof(logger_event_t);
        if (nvs_get_blob(handle, key, &events[count], &len) == ESP_OK) {
            count++;
        }
    }

    *actual_count = count;
    nvs_close(handle);
    return ESP_OK;
}

esp_err_t logger_clear_all(void) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition("logger", LOGGER_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    for (int i = 0; i < LOGGER_EVENT_MAX; i++) {
        char key[16];
        snprintf(key, sizeof(key), "event_%02d", i);
        nvs_erase_key(handle, key);
    }
    nvs_erase_key(handle, "write_index");

    nvs_commit(handle);
    nvs_close(handle);
    return ESP_OK;
}

