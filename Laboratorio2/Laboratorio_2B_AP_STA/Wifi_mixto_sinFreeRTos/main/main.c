#include <string.h>

#include "esp_system.h"  // Para esp_task_wdt_reset()
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_mac.h"
#include "inttypes.h"

// Para busy-wait y funciones de tiempo a bajo nivel
#include "esp_timer.h"     // Para esp_timer_get_time() (aunque no es esp_rom_get_high_speed_timer)
#include "esp_rom_sys.h"
#include "inttypes.h"

// Para busy-wait, usaremos una función de retardo básica
#include "esp_timer.h" // Para esp_rom_delay_us o funciones de tiempo más precisas (sin FreeRTOS)
#include "rom/ets_sys.h" // Para ets_delay_us (busy-wait) o esp_rom_delay_us

// --- Configuración del Access Point (AP) ---
#define AP_SSID "ESP32-S2_Marcos"
#define AP_PASSWORD "123456789"
#define AP_CHANNEL          6
#define AP_MAX_CONNECTIONS  4

// --- Configuración de la Estación (STA) ---
#define STA_SSID "caliope"
#define STA_PASSWORD "sinlugar"
#define STA_MAX_RETRY       5

static const char *TAG = "WIFI_AP_STA_MIXED";

static int s_retry_num = 0;

// --- Variables globales para señalizar el estado de la conexión STA ---
// Estas variables reemplazan el Event Group.
volatile bool sta_connected = false;
volatile bool sta_failed = false;

#define WATCHDOG_RESET_INTERVAL_MS 2000 // Resetear el watchdog cada 2 segundos (2000 ms)

void delay_ms_busy_wait(uint32_t ms) {
    uint64_t start_time_us = esp_rom_get_high_speed_timer();
    uint64_t end_time_us = start_time_us + (uint64_t)ms * 1000;
    uint64_t next_wdt_reset_time_us = start_time_us + (uint64_t)WATCHDOG_RESET_INTERVAL_MS * 1000;

    while (esp_rom_get_high_speed_timer() < end_time_us) {
        // Aquí puedes agregar lógica si necesitas hacer algo durante el retardo
        // Por ejemplo, verificar un GPIO, o procesar un buffer pequeño si es muy crítico.

        // Resetear el watchdog si ha pasado el intervalo definido
        if (esp_rom_get_high_speed_timer() >= next_wdt_reset_time_us) {
            esp_task_wdt_reset(); // Reinicia el contador del watchdog para la tarea actual
            next_wdt_reset_time_us += (uint64_t)WATCHDOG_RESET_INTERVAL_MS * 1000;
        }
    }
}

// --- Manejador de Eventos Unificado para AP y STA ---
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_AP_STACONNECTED) {
            wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
            ESP_LOGI(TAG, "AP: Estacion conectada. MAC: " MACSTR ", AID=%d",
                     MAC2STR(event->mac), event->aid);
        } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
            wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
            ESP_LOGI(TAG, "AP: Estacion desconectada. MAC: " MACSTR ", AID=%d, Razon=%d",
                     MAC2STR(event->mac), event->aid, event->reason);
        }
        else if (event_id == WIFI_EVENT_STA_START) {
            esp_wifi_connect();
            ESP_LOGI(TAG, "STA: Iniciada. Intentando conectar a la red: %s...", STA_SSID);
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            wifi_event_sta_disconnected_t* event = (wifi_event_sta_disconnected_t*) event_data;
            if (s_retry_num < STA_MAX_RETRY) {
                esp_wifi_connect();
                s_retry_num++;
                ESP_LOGI(TAG, "STA: Reconectando al AP. Reintento #%d...", s_retry_num);
            } else {
                sta_failed = true; // Señalizar fallo
                ESP_LOGE(TAG, "STA: Fallo al conectar despues de %d reintentos.", STA_MAX_RETRY);
            }
            sta_connected = false; // Asegurar que el estado es desconectado
            ESP_LOGI(TAG, "STA: Desconectada (razon: %" PRIu8 ").", event->reason);
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "STA: Conectada y obtuvo IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0; // Resetear el contador de reintentos al conectar exitosamente
        sta_connected = true; // Señalizar conexión exitosa
        sta_failed = false;   // Resetear el flag de fallo
    }
}

// --- Función de Inicialización WiFi para AP y STA ---
void wifi_init_apsta(void)
{
    // NO se crea Event Group aquí
    // NO se usan funciones de FreeRTOS

    // 1. Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Inicializar el stack TCP/IP y el Event Loop por defecto.
    // Aunque estas funciones internamente pueden usar FreeRTOS,
    // tu código no las llama explícitamente desde FreeRTOS.
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 3. Crear instancias de las interfaces de red por defecto para AP y STA.
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    // 4. Inicializar el controlador WiFi con la configuración por defecto.
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 5. Registrar el manejador de eventos unificado para eventos WiFi y IP.
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    // 6. Iniciar el WiFi en modo dual (AP + STA) ANTES de configurar los detalles de cada modo.
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

    // 7. Configurar el modo AP (Access Point).
    wifi_config_t ap_config = {
        .ap = {
            .ssid = AP_SSID,
            .password = AP_PASSWORD,
            .ssid_len = strlen(AP_SSID),
            .channel = AP_CHANNEL,
            .max_connection = AP_MAX_CONNECTIONS,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .required = false,
            },
        },
    };
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
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));

    // 9. Iniciar el WiFi.
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi AP+STA iniciado.");
    ESP_LOGI(TAG, "AP: SSID '%s', Canal: %d, Contraseña: %s", AP_SSID, AP_CHANNEL, (strlen(AP_PASSWORD) > 0 ? "Configurada" : "Abierta"));
    ESP_LOGI(TAG, "STA: Intentando conectar a: '%s'...", STA_SSID);

    // Esperar hasta que la STA se conecte o falle usando busy-wait
    ESP_LOGI(TAG, "STA: Esperando por IP o fallo de conexion (¡usando busy-wait!)...");
    // Aquí es donde el busy-wait puede ser problemático.
    // Daremos un tiempo máximo de espera razonable para no bloquear indefinidamente.
    // Se recomienda un tiempo máximo de 30 segundos (30000ms) para la conexión WiFi.
    uint32_t start_time = esp_timer_get_time() / 1000; // Tiempo en ms
    uint32_t timeout_ms = 30000; // 30 segundos

    while (!sta_connected && !sta_failed) {
        // Chequear timeout para evitar bloqueo infinito
        if ((esp_timer_get_time() / 1000) - start_time > timeout_ms) {
            ESP_LOGE(TAG, "STA: Tiempo de espera agotado para la conexion.");
            sta_failed = true; // Forzar el fallo si hay timeout
            break;
        }
        // Este delay_ms_busy_wait es el que BLOQUEA la CPU.
        // Un valor pequeño (ej. 10ms) hará que se sondee más rápido,
        // pero seguirá siendo un busy-wait.
        delay_ms_busy_wait(10); // Espera de 10ms en cada iteración del bucle
    }

    if (sta_connected) {
        ESP_LOGI(TAG, "STA: Conectado a la red externa!");
    } else if (sta_failed) {
        ESP_LOGE(TAG, "STA: Fallo al conectar a la red externa.");
    } else {
        ESP_LOGE(TAG, "STA: Estado inesperado al salir del bucle.");
    }
}

// --- Función principal de la aplicación ---
void app_main(void) {
    // ... tu código de inicialización de WiFi y NVS Flash ...

    ESP_LOGI(TAG, "APLICACION: Bucle principal activo (usando busy-wait con WDT reset).");
    while(1) {
        // Tu lógica principal aquí
        // ...

        delay_ms_busy_wait(100); // Esto ahora llamará a tu función modificada
                                 // que se encarga de resetear el WDT
    }
}