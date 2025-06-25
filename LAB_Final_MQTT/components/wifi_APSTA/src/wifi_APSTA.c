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
static bool sta_connected = false;


static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "STA desconectado. Reintentando conexión...");
        esp_wifi_connect();
        sta_connected = false;
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        sta_connected = true;
        ESP_LOGI(TAG, "STA obtuvo IP. Conectado a red.");
    }
}


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
            .ssid = "ESP32_AP",
            .password = "12345678",
            .ssid_len = strlen("ESP32_AP"),
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };

    // Configuración del cliente STA
    wifi_config_t sta_config = {
        .sta = {
            .ssid = "Gaston_y_Lu",
            .password = "Pintorescos1414"
        }
    };

    // Modo dual AP + STA y aplicación de configuraciones
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

    ESP_LOGI(TAG, "WiFi inicializado en modo AP+STA");
}

bool wifi_sta_conectado(void) { //Devuelve el estado de conexion
    return sta_connected;
}

void wifi_apsta_bucle(void) {
    // Falta esta parte
}
