#include <string.h>
#include <stdbool.h>
#include "wifi_APSTA.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_system.h"


static const char *TAG = "WIFI_APSTA";
static TaskHandle_t wifi_task_handle = NULL;

#define MAX_STA_RETRIES 3
#define WIFI_NAMESPACE "wifi_config"
static bool sta_connected = false;

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
    }
}

//-----------------------------
// Lee las credenciales del NVS
//-----------------------------
static bool nvs_leer_credenciales(char *ssid, char *password) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(WIFI_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) return false;

    size_t ssid_len = 32;
    size_t pass_len = 64;

    err = nvs_get_str(nvs_handle, "ssid", ssid, &ssid_len);
    if (err != ESP_OK) {
        nvs_close(nvs_handle);
        return false;
    }

    err = nvs_get_str(nvs_handle, "pass", password, &pass_len);
    nvs_close(nvs_handle);

    return err == ESP_OK;
}

//-------------------
//Tarea de conexión STA (bloqueante, espera cambios)
//-------------------
static void wifi_sta_task(void *param) {
    char ssid[32];
    char pass[64];

    while (true) {
        if (!nvs_leer_credenciales(ssid, pass)) {
            ESP_LOGW(TAG, "Esperando credenciales en NVS...");
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        wifi_config_t sta_config = {0};
        strncpy((char *)sta_config.sta.ssid, ssid, sizeof(sta_config.sta.ssid));
        strncpy((char *)sta_config.sta.password, pass, sizeof(sta_config.sta.password));

        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));

        for (int intentos = 0; intentos < MAX_STA_RETRIES; ++intentos) {
            ESP_LOGI(TAG, "Intento de conexión STA (%d/3)...", intentos + 1);
            esp_wifi_connect();

            int wait = 0;
            while (!sta_connected && wait++ < 20) {  // Espera hasta 2 segundos
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
            ESP_LOGE(TAG, "No se pudo conectar. Reintentando cuando haya nuevas credenciales...");
        }

        vTaskDelay(pdMS_TO_TICKS(10000)); // Espera antes de reintentar o seguir conectado
    }
}

//-------------------
//Inicializar WiFi
//-------------------
void wifi_apsta_inicializar(void) {
    // Inicializar NVS de forma segura
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // Crear interfaces de red
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    // Inicializar Wi-Fi con configuración por defecto
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Registrar handlers de eventos
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    // Configuración del punto de acceso
    wifi_config_t ap_config = {
        .ap = {
            .ssid = "Luis_AP",
            .password = "12345678",
            .ssid_len = strlen("Luis_AP"),
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };

    // Configuración del cliente STA
    wifi_config_t sta_config = {
        .sta = {
            .ssid = "Usuario",
            .password = "Contraseña"

        }
    };
    
    // Aplicamos configuración solo al AP
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));

    // Iniciar WiFi (en modo AP+STA)
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_netif_ip_info_t ap_ip;
    esp_netif_t *ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    if (ap_netif && esp_netif_get_ip_info(ap_netif, &ap_ip) == ESP_OK) {
        ESP_LOGI(TAG, "AP IP: " IPSTR, IP2STR(&ap_ip.ip));
    }
    // Lanzamos la tarea que intenta conectar al STA cuando haya credenciales
    xTaskCreate(wifi_sta_task, "wifi_sta_task", 4096, NULL, 5, &wifi_task_handle);

    ESP_LOGI(TAG, "WiFi inicializado en modo AP+STA");
}


bool wifi_sta_conectado(void) {
    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (netif == NULL) return false;
    esp_netif_ip_info_t ip_info;
    if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
        return (ip_info.ip.addr != 0);
    }
    return false;
}

void wait_for_wifi(void) {
    while (!wifi_sta_conectado()) {
        ESP_LOGI("WIFI", "Esperando conexión Wi-Fi...");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    ESP_LOGI("WIFI", "Wi-Fi conectada, continuamos...");
}

//-------------------
//Inicializar WiFi
//-------------------
void wifi_apsta_bucle(void) {
    // Falta esta parte
}