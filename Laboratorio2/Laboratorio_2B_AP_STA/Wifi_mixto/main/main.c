#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h" // Para EventGroup
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h" // Necesario para MACSTR, IPSTR, MAC2STR, IP2STR
#include "esp_mac.h"   // Para MACSTR (en algunas versiones de IDF)
#include "inttypes.h"  // Necesario para PRId32, PRIu32, etc.

#include "lwip/err.h"
#include "lwip/sys.h"

// --- Configuración del Access Point (AP) ---
// NOTA: Estos valores no se leerán de sdkconfig.h, sino directamente de aquí.
#define AP_SSID "ESP32-S2_MyAP" // SSID de tu punto de acceso (visible para otros dispositivos)
#define AP_PASSWORD "123456789"     // Contraseña de tu punto de acceso (mín. 8 caracteres para WPA2)
#define AP_CHANNEL          6               // Canal WiFi para el AP (1-13)
#define AP_MAX_CONNECTIONS  4               // Máximo de estaciones que pueden conectarse a este AP

// --- Configuración de la Estación (STA) ---
// ¡¡¡IMPORTANTE!!! Reemplaza estos valores con el SSID y la contraseña de tu red Wi-Fi
#define STA_SSID "Anteltcbp9-2.4GHz"    // SSID de tu red Wi-Fi (a la que el ESP32 se conectará)
#define STA_PASSWORD "ssxug473" // Contraseña de tu red Wi-Fi
#define STA_MAX_RETRY       5              // Número máximo de reintentos para la conexión STA

// --- Event Group para la conexión STA ---
// Se usa para señalizar el estado de la conexión STA a la red externa.
static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0 // Bit que se establece cuando la STA se conecta y obtiene IP
#define WIFI_FAIL_BIT      BIT1 // Bit que se establece cuando la STA falla en conectar después de reintentos

static const char *TAG = "WIFI_AP_STA_MIXED"; // Tag unificado para los logs

static int s_retry_num = 0; // Contador de reintentos de conexión STA

// --- Manejador de Eventos Unificado para AP y STA ---
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    // --- Eventos del modo AP (Access Point) ---
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_AP_STACONNECTED) {
            wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
            // Un dispositivo se ha conectado al AP del ESP32
            ESP_LOGI(TAG, "AP: Estacion conectada. MAC: " MACSTR ", AID=%d",
                     MAC2STR(event->mac), event->aid);
        } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
            wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
            // Un dispositivo se ha desconectado del AP del ESP32
            ESP_LOGI(TAG, "AP: Estacion desconectada. MAC: " MACSTR ", AID=%d, Razon=%d",
                     MAC2STR(event->mac), event->aid, event->reason);
        }
        // --- Eventos del modo STA (Estación) ---
        else if (event_id == WIFI_EVENT_STA_START) {
            // La interfaz STA se ha iniciado, intentar conectar a la red externa
            esp_wifi_connect();
            ESP_LOGI(TAG, "STA: Iniciada. Intentando conectar a la red: %s...", STA_SSID);
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            wifi_event_sta_disconnected_t* event = (wifi_event_sta_disconnected_t*) event_data;
            // La STA se ha desconectado de la red. Reintentar conexión.
            if (s_retry_num < STA_MAX_RETRY) {
                esp_wifi_connect();
                s_retry_num++;
                ESP_LOGI(TAG, "STA: Reconectando al AP. Reintento #%d...", s_retry_num);
            } else {
                xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT); // Señalizar fallo
                ESP_LOGE(TAG, "STA: Fallo al conectar despues de %d reintentos.", STA_MAX_RETRY);
            }
            ESP_LOGI(TAG, "STA: Desconectada (razon: %" PRIu8 ").", event->reason); // Usar PRIu8
        }
    }
    // --- Eventos de IP (principalmente para el modo STA) ---
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        // La STA ha obtenido una dirección IP de la red externa a la que se conectó
        ESP_LOGI(TAG, "STA: Conectada y obtuvo IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0; // Resetear el contador de reintentos al conectar exitosamente
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT); // Señalizar conexión exitosa
    }
}

// --- Función de Inicialización WiFi para AP y STA ---
void wifi_init_apsta(void)
{
    // Inicializar Event Group para la STA
    s_wifi_event_group = xEventGroupCreate();

    // 1. Inicializar NVS (Non-Volatile Storage) para almacenamiento de configuración WiFi.
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase()); // Borra la NVS si no hay espacio o si hay una versión nueva
        ret = nvs_flash_init();             // Vuelve a inicializar
    }
    ESP_ERROR_CHECK(ret); // Verifica que la inicialización de NVS sea exitosa

    // 2. Inicializar el stack TCP/IP de LwIP y el Event Loop por defecto.
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 3. Crear instancias de las interfaces de red por defecto para AP y STA.
    // Estas funciones preparan las interfaces de red para ambos modos.
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    // 4. Inicializar el controlador WiFi con la configuración por defecto.
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 5. Registrar el manejador de eventos unificado para eventos WiFi y IP.
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID, // Para todos los eventos WiFi
                                                        &event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP, // Solo para el evento de IP obtenida por STA
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    // AHORA VIENE EL ORDEN CORRECTO:
    // 6. Iniciar el WiFi en modo dual (AP + STA) ANTES de configurar los detalles de cada modo.
    // Esto es crucial para que esp_wifi_set_config funcione correctamente.
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA)); // Mover esta línea aquí

    // 7. Configurar el modo AP (Access Point).
    wifi_config_t ap_config = {
        .ap = {
            .ssid = AP_SSID,
            .password = AP_PASSWORD,
            .ssid_len = strlen(AP_SSID),
            .channel = AP_CHANNEL,
            .max_connection = AP_MAX_CONNECTIONS,
            .authmode = WIFI_AUTH_WPA2_PSK, // Usar WPA2-PSK para seguridad
            .pmf_cfg = {
                .required = false, // Puedes establecerlo a true si tu AP requiere PMF
            },
        },
    };
    // Si la contraseña del AP está vacía, se configura como red abierta.
    if (strlen(AP_PASSWORD) == 0) {
        ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));

    // 8. Configurar el modo STA (Estación).
    wifi_config_t sta_config = {
        .sta = {
            .ssid = STA_SSID,
            .password = STA_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config)); // Esta línea ahora está en el lugar correcto

    // 9. Iniciar el WiFi.
    ESP_ERROR_CHECK(esp_wifi_start()); // Mantener esta línea después de todas las configuraciones

    ESP_LOGI(TAG, "WiFi AP+STA iniciado.");
    ESP_LOGI(TAG, "AP: SSID '%s', Canal: %d, Contraseña: %s", AP_SSID, AP_CHANNEL, (strlen(AP_PASSWORD) > 0 ? "Configurada" : "Abierta"));
    ESP_LOGI(TAG, "STA: Intentando conectar a: '%s'...", STA_SSID);

    // Esperar hasta que la STA se conecte o falle
    ESP_LOGI(TAG, "STA: Esperando por IP o fallo de conexion...");
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "STA: Conectado a la red externa!");
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(TAG, "STA: Fallo al conectar a la red externa.");
    } else {
        ESP_LOGE(TAG, "STA: Evento inesperado en la espera.");
    }
}

// --- Función principal de la aplicación ---
void app_main(void)
{
    // ... (el código de app_main se mantiene igual) ...
    wifi_init_apsta(); // Llamar a la función de inicialización AP+STA
}