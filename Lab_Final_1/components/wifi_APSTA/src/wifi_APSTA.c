#include "wifi_APSTA.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "shared_lib.h"
#include "task_mqtt.h"
#include "ntp.h"
#include "logger.h"
#include "freertos/event_groups.h"


static EventGroupHandle_t wifi_event_group;
#define WIFI_EVENT_FLAG_NEW_CREDENTIALS BIT0
#define TAG "WIFI_APSTA"
#define MAX_STA_RETRIES 3

static wifi_state_t current_state = WIFI_STATE_INIT;
static int retry_count = 0;
static bool mqtt_started = false;
static TaskHandle_t wifi_fsm_task_handle = NULL;
static bool sta_connected = false;

//------------------------------
// Handler de eventos WiFi/IP
//------------------------------

void wifi_modo_ap_puro(void) {
    ESP_LOGI(TAG, "Reiniciando en modo AP puro");
    esp_wifi_stop();
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_start();
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_DISCONNECTED: {
                sta_connected = false;
                mqtt_started = false;
                retry_count++;

                wifi_event_sta_disconnected_t *disc = (wifi_event_sta_disconnected_t *)event_data;
                ESP_LOGW(TAG, "STA desconectado. Reintento #%d, motivo: %d", retry_count, disc->reason);

                if (retry_count >= MAX_STA_RETRIES) {
                    current_state = WIFI_STATE_WAITING_USER;
                    retry_count = 0;
                } else {
                    current_state = WIFI_STATE_CONNECTING;
                    esp_wifi_connect();
                }
                break;
            }

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
        current_state = WIFI_STATE_CONNECTED;

        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "STA obtuvo IP: " IPSTR, IP2STR(&event->ip_info.ip));

        ntp_sync_inicializar();

        if (!mqtt_started) {
            if (task_mqtt_start(NULL) == ESP_OK) {
                mqtt_started = true;
                logger_publicar_al_arrancar();
            }
        }

        if (wifi_fsm_task_handle != NULL) {
            vTaskDelete(wifi_fsm_task_handle);
            wifi_fsm_task_handle = NULL;
        }
    }
}

//------------------------------
// Tarea FSM
//------------------------------

void wifi_fsm_notificar_nuevas_credenciales(void) {
    xEventGroupSetBits(wifi_event_group, WIFI_EVENT_FLAG_NEW_CREDENTIALS);
}


void wifi_fsm_task(void *param) {
    wifi_credentials_t cred;
    int reintentos = 0;
    const int MAX_REINTENTOS = 3;

    while (1) {
        switch (current_state) {
            case WIFI_STATE_INIT:
                if (!wifi_credentials_leer(&cred)) {
                    current_state = WIFI_STATE_WAIT_CREDENTIALS;
                } else {
                    current_state = WIFI_STATE_CONNECTING;
                }
                break;

            case WIFI_STATE_WAIT_CREDENTIALS:
                ESP_LOGW(TAG, "Esperando credenciales en NVS...");
                vTaskDelay(pdMS_TO_TICKS(10000));
                if (wifi_credentials_leer(&cred)) {
                    current_state = WIFI_STATE_CONNECTING;
                }
                break;

            case WIFI_STATE_CONNECTING:
                if (!wifi_credentials_validas(&cred)) {
                    ESP_LOGW(TAG, "Credenciales inválidas. Esperando...");
                    vTaskDelay(pdMS_TO_TICKS(10000));
                    break;
                }

                ESP_LOGI(TAG, "Intentando conectar a SSID: '%s'", cred.ssid);

                wifi_config_t sta_config = {0};
                strncpy((char *)sta_config.sta.ssid, cred.ssid, sizeof(sta_config.sta.ssid) - 1);
                strncpy((char *)sta_config.sta.password, cred.password, sizeof(sta_config.sta.password) - 1);
                sta_config.sta.threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK;

                ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
                esp_wifi_connect();
                vTaskDelay(pdMS_TO_TICKS(10000));

                // Revisa si estamos conectados, si no reintenta
                wifi_ap_record_t ap_info;
                if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
                    ESP_LOGI(TAG, "Conexión exitosa al AP: %s", ap_info.ssid);
                    current_state = WIFI_STATE_CONNECTED;
                    reintentos = 0;  // resetea reintentos al conectarse
                } else {
                    reintentos++;
                    ESP_LOGW(TAG, "Fallo de conexión #%d", reintentos);
                    if (reintentos >= MAX_REINTENTOS) {
                        ESP_LOGE(TAG, "Demasiados reintentos. Volviendo a modo AP puro.");
                        wifi_modo_ap_puro();
                        current_state = WIFI_STATE_WAITING_USER;
                        reintentos = 0;
                    }
                }
                break;

            case WIFI_STATE_CONNECTED:
                vTaskDelay(pdMS_TO_TICKS(10000));
                break;

            case WIFI_STATE_WAITING_USER:
                ESP_LOGW(TAG, "Esperando nuevas credenciales del usuario...");

                EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                                       WIFI_EVENT_FLAG_NEW_CREDENTIALS,
                                                       pdTRUE, pdFALSE,
                                                       pdMS_TO_TICKS(600000));  // 10 minutos

                if (bits & WIFI_EVENT_FLAG_NEW_CREDENTIALS) {
                    ESP_LOGI(TAG, "Evento de nuevas credenciales recibido.");

                    wifi_credentials_t nuevas_credenciales = {0};
                    if (wifi_credentials_cargar(&nuevas_credenciales)) {
                        ESP_LOGI(TAG, "Intentando conectar a SSID: '%s'", nuevas_credenciales.ssid);
                        cred = nuevas_credenciales;  // actualiza credenciales en uso
                        current_state = WIFI_STATE_CONNECTING;
                    } else {
                        ESP_LOGE(TAG, "Error al cargar nuevas credenciales desde NVS.");
                        wifi_modo_ap_puro();
                        current_state = WIFI_STATE_WAITING_USER;
                    }
                }
                break;

            default:
                current_state = WIFI_STATE_INIT;
                break;
        }
    }
}

//------------------------------
// Inicialización FSM
//------------------------------
void wifi_fsm_inicializar(void) {
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
    xTaskCreate(wifi_fsm_task, "wifi_fsm_task", 4096, NULL, 5, &wifi_fsm_task_handle);
}

//------------------------------
// Inicialización general WiFi
//------------------------------
void wifi_apsta_inicializar(void) {
    ESP_LOGI(TAG, "Inicializando WiFi...");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    wifi_event_group = xEventGroupCreate();
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

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

    wifi_fsm_inicializar();
    ESP_LOGI(TAG, "WiFi inicializado en modo AP+STA");
}
