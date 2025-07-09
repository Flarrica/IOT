#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "shared_lib.h"
#include "led_rgb.h"
#include "esp_log.h"
#include "nvs.h"
#include <string.h>
#include "esp_spiffs.h" 
#include "esp_check.h"

static const char *TAG = "shared_lib";

// Definición de las variables globales
SemaphoreHandle_t color_semaphore = NULL;
SemaphoreHandle_t timer_done_semaphore = NULL;
led_rgb_evento_t current_color = LED_EVENT_APAGAR;
SemaphoreHandle_t i2c_mutex = NULL;
QueueHandle_t io_mutex = NULL;
QueueHandle_t color_queue = NULL;
SemaphoreHandle_t spiffs_mutex = NULL;


bool wifi_credentials_guardar(const wifi_credentials_t *cred) {
    nvs_handle_t handle;
    if (nvs_open(WIFI_NAMESPACE, NVS_READWRITE, &handle) != ESP_OK) return false;

    esp_err_t err = nvs_set_str(handle, "ssid", cred->ssid);
    if (err == ESP_OK) err = nvs_set_str(handle, "pass", cred->password);
    if (err == ESP_OK) err = nvs_commit(handle);

    nvs_close(handle);
    return err == ESP_OK;
}

bool wifi_credentials_leer(wifi_credentials_t *cred) {
    nvs_handle_t handle;
    if (nvs_open(WIFI_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) return false;

    size_t ssid_len = sizeof(cred->ssid);
    size_t pass_len = sizeof(cred->password);
    esp_err_t err1 = nvs_get_str(handle, "ssid", cred->ssid, &ssid_len);
    esp_err_t err2 = nvs_get_str(handle, "pass", cred->password, &pass_len);

    nvs_close(handle);
    return (err1 == ESP_OK && err2 == ESP_OK);
}

bool wifi_credentials_validas(const wifi_credentials_t *cred) {
    return (cred && strlen(cred->ssid) > 0 && strlen(cred->password) >= 8);
}

// ------------------------
// MONTAJE DE SPIFFS
// ------------------------

esp_err_t spiffs_init(void) {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "spiffs",
        .max_files = 10,
        .format_if_mount_failed = false
    };

    ESP_RETURN_ON_ERROR(esp_vfs_spiffs_register(&conf), TAG, "Fallo al montar SPIFFS");

    size_t total = 0, used = 0;
    esp_err_t info_ret = esp_spiffs_info("spiffs", &total, &used);
    if (info_ret == ESP_OK) {
        ESP_LOGI(TAG, "SPIFFS montado: %d KB totales, %d KB usados", total / 1024, used / 1024);
    } else {
        ESP_LOGW(TAG, "No se pudo obtener información de SPIFFS (err=0x%x)", info_ret);
    }

    return ESP_OK;
}

void inicializar_recursos_globales(void) {
    color_semaphore = xSemaphoreCreateBinary();
    timer_done_semaphore = xSemaphoreCreateBinary();
    color_queue = xQueueCreate(50, sizeof(color_event_t));
    // Chequear que todo este inicializado y sino, dar ERROR
    if (color_semaphore == NULL || timer_done_semaphore == NULL || color_queue == NULL) {
        ESP_LOGE("SHARED", "Error creando recursos globales");
        abort();
    }
}

bool wifi_credentials_borrar(void) {
    nvs_handle_t handle;
    if (nvs_open(WIFI_NAMESPACE, NVS_READWRITE, &handle) != ESP_OK) return false;

    nvs_erase_key(handle, "ssid");
    nvs_erase_key(handle, "pass");

    esp_err_t err = nvs_commit(handle);
    nvs_close(handle);
    return err == ESP_OK;
}

esp_err_t mqtt_borrar_url(void) {
    nvs_handle_t handle;
    if (nvs_open("mqtt_config", NVS_READWRITE, &handle) != ESP_OK) return ESP_FAIL;
    nvs_erase_key(handle, "broker_url");
    esp_err_t err = nvs_commit(handle);
    nvs_close(handle);
    return err;
}