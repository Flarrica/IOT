#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_netif.h" // Necesario para MACSTR, IPSTR
#include "inttypes.h"  // Necesario para PRId, PRIu32, etc.

// Configuración AP
#define AP_SSID "ESP32-S2_AP"
#define AP_PASSWORD "123456789"
#define AP_CHANNEL           6
#define AP_MAX_CONNECTIONS   4

// Configuración STA
#define STA_SSID             "TuRedWiFi" // ¡IMPORTANTE! Reemplaza con el SSID de tu red
#define STA_PASSWORD         "TuContraseña" // ¡IMPORTANTE! Reemplaza con la contraseña de tu red
#define STA_MAX_RETRIES      5 // Esta variable no se usa en el código actual, pero la mantengo si la piensas usar.

static const char *TAG = "wifi_ap_sta";

// Manejador de eventos corregido
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                              int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_AP_STACONNECTED) {
            wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
            // Corrección: Usar MAC2STR para expandir la dirección MAC en los argumentos requeridos por MACSTR
            ESP_LOGI(TAG, "Dispositivo conectado al AP! MAC: " MACSTR ", AID=%" PRId32,
                     MAC2STR(event->mac), // <-- ¡CORREGIDO!
                     event->aid);
        } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
            wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
            // Corrección: Usar MAC2STR para expandir la dirección MAC en los argumentos requeridos por MACSTR
            ESP_LOGI(TAG, "Dispositivo desconectado del AP. MAC: " MACSTR ", AID=%" PRId32,
                     MAC2STR(event->mac), // <-- ¡CORREGIDO!
                     event->aid);
        } else if (event_id == WIFI_EVENT_STA_START) {
            esp_wifi_connect();
            ESP_LOGI(TAG, "STA iniciada. Conectando a %s...", STA_SSID);
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            wifi_event_sta_disconnected_t* event = (wifi_event_sta_disconnected_t*) event_data;
            ESP_LOGW(TAG, "STA desconectada (razón: %" PRId32 "). Reconectando...", event->reason);
            // Puedes añadir lógica para reintentos aquí si es necesario, usando STA_MAX_RETRIES
            esp_wifi_connect();
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        // Corrección: Usar IP2STR para expandir la dirección IP en los argumentos requeridos por IPSTR
        ESP_LOGI(TAG, "STA conectada! IP: " IPSTR, IP2STR(&event->ip_info.ip)); // <-- ¡CORREGIDO!
    }
}

void wifi_init_apsta(void) {
    // 1. Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Inicializar stack TCP/IP y evento loop
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 3. Crear interfaces AP y STA
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    // 4. Configurar WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 5. Registrar manejador de eventos
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    // 6. Configurar AP
    wifi_config_t ap_config = {
        .ap = {
            .ssid = AP_SSID,
            .password = AP_PASSWORD,
            .ssid_len = strlen(AP_SSID),
            .channel = AP_CHANNEL,
            .max_connection = AP_MAX_CONNECTIONS,
            .authmode = WIFI_AUTH_WPA2_PSK
        }
    };
    if (strlen(AP_PASSWORD) == 0) {
        ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));

    // 7. Configurar STA
    wifi_config_t sta_config = {
        .sta = {
            .ssid = STA_SSID,
            .password = STA_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK // Considera WIFI_AUTH_OPEN si tu red no tiene contraseña
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));

    // 8. Iniciar WiFi en modo dual
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "AP+STA iniciado. AP SSID: %s, Canal: %d", AP_SSID, AP_CHANNEL);
    ESP_LOGI(TAG, "Intentando conectar STA a: %s...", STA_SSID);
}

void app_main(void) {
    wifi_init_apsta();
}