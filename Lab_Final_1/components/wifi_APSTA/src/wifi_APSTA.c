#include <string.h>
#include <stdbool.h>
#include "wifi_APSTA.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_system.h"

#include "task_mqtt.h"
#include "shared_lib.h"
#include "ntp.h"
#include "logger.h"

static const char *TAG = "WIFI_APSTA";

static TaskHandle_t wifi_task_handle = NULL;

#define MAX_STA_RETRIES 3
static bool sta_connected = false;
static bool mqtt_started = false;

//------------------
// Handler de WiFi
//------------------
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGW(TAG, "STA desconectado. Reintentando conexión...");
                esp_wifi_connect();
                sta_connected = false;
                mqtt_started = false;

                if (wifi_task_handle == NULL) {
                    xTaskCreate(wifi_sta_task, "wifi_sta_task", 4096, NULL, 5, &wifi_task_handle);
                    ESP_LOGI(TAG, "Tarea wifi_sta_task recreada tras desconexión STA.");
                }
                break;

            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "AP iniciado. Esperando conexiones...");
                break;

            case WIFI_EVENT_AP_STACONNECTED:
                ESP_LOGI(TAG, "Un dispositivo se conectó al AP.");
                break;

            case WIFI_EVENT_AP_STADISCONNECTED:
                ESP_LOGI(TAG, "Un dispositivo se desconectó del AP.");
                break;
        }

        } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
            sta_connected = true;
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            ESP_LOGI(TAG, "STA obtuvo IP. Conectado a red: " IPSTR, IP2STR(&event->ip_info.ip));

            // 🔽 NUEVO: sincronizar hora con SNTP
            ntp_sync_inicializar();

            if (!mqtt_started) {
                ESP_LOGI(TAG, "Iniciando cliente MQTT...");
                if (task_mqtt_start(NULL) == ESP_OK) {
                    mqtt_started = true;
                    ESP_LOGI(TAG, "Cliente MQTT iniciado correctamente.");

                    // 🔽 Publicar logger al conectarse
                    logger_publicar_al_arrancar();
                } else {
                    ESP_LOGE(TAG, "Error al iniciar cliente MQTT.");
                }
            }
            if (wifi_task_handle != NULL) {
                vTaskDelete(wifi_task_handle);
                wifi_task_handle = NULL;
                ESP_LOGI(TAG, "Tarea wifi_sta_task eliminada tras conexión exitosa STA.");
            }
        }

}


//-------------------
// Tarea de conexión STA
//-------------------
void wifi_sta_task(void *param) {
    wifi_credentials_t cred;

    while (true) {
        if (sta_connected) {
            vTaskDelay(pdMS_TO_TICKS(10000));
            continue;
        }

        if (!wifi_credentials_leer(&cred)) {
            ESP_LOGW(TAG, "Esperando credenciales en NVS...");
            vTaskDelay(pdMS_TO_TICKS(10000));
            continue;
        }

        if (!wifi_credentials_validas(&cred)) {
            ESP_LOGW(TAG, "Credenciales inválidas. Esperando...");
            vTaskDelay(pdMS_TO_TICKS(10000));
            continue;
        }

        wifi_config_t sta_config = {0};
        strncpy((char *)sta_config.sta.ssid, cred.ssid, sizeof(sta_config.sta.ssid));
        strncpy((char *)sta_config.sta.password, cred.password, sizeof(sta_config.sta.password));

        esp_err_t err = esp_wifi_set_config(WIFI_IF_STA, &sta_config);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error al guardar config STA: %s", esp_err_to_name(err));

        }

        for (int intento = 0; intento < MAX_STA_RETRIES; ++intento) {
            ESP_LOGI(TAG, "Intento de conexión STA (%d/%d)...", intento + 1, MAX_STA_RETRIES);
            esp_wifi_connect();

            int espera = 0;
            while (!sta_connected && espera++ < 20) {
                vTaskDelay(pdMS_TO_TICKS(100));
            }

            if (sta_connected) {
                ESP_LOGI(TAG, "Conexión STA exitosa.");
                break;
            } else {
                ESP_LOGW(TAG, "Fallo en la conexión STA.");
            }
        }

        if (!sta_connected) {
            ESP_LOGE(TAG, "No se pudo conectar. Reintentando...");
        }

        if (!sta_connected) {
            ESP_LOGW(TAG, "Fallo en la conexión STA. Reintentando...");
            esp_wifi_set_mode(WIFI_MODE_APSTA);  // Reafirmar el modo
            esp_wifi_start();  // Asegura que AP siga activo
        }

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

//-------------------
// Inicializar WiFi
//-------------------
void wifi_apsta_inicializar(void) {
    ESP_LOGI(TAG, "Inicializando logger...");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = "Luis_AP",
            .password = "12345678",
            .ssid_len = strlen("Luis_AP"),
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_netif_ip_info_t ap_ip;
    esp_netif_t *ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    if (ap_netif && esp_netif_get_ip_info(ap_netif, &ap_ip) == ESP_OK) {
        ESP_LOGI(TAG, "AP IP: " IPSTR, IP2STR(&ap_ip.ip));
    }

    xTaskCreate(wifi_sta_task, "wifi_sta_task", 4096, NULL, 5, &wifi_task_handle);
    ESP_LOGI(TAG, "WiFi inicializado en modo AP+STA");
}