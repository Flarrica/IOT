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
#include "freertos/portmacro.h"

static EventGroupHandle_t wifi_event_group;
#define WIFI_EVENT_FLAG_NEW_CREDENTIALS BIT0
#define TAG "WIFI_APSTA"
#define MAX_STA_RETRIES 3

static wifi_state_t current_state = WIFI_STATE_INIT;
static bool mqtt_started = false;
static TaskHandle_t wifi_fsm_task_handle = NULL;
static bool sta_connected = false;

// Agregar al principio

static portMUX_TYPE state_mux = portMUX_INITIALIZER_UNLOCKED;
//------------------------------
// Handler de eventos WiFi/IP
//------------------------------

void wifi_modo_ap_puro(void) {
    ESP_LOGI(TAG, "Reiniciando en modo AP puro");
    esp_wifi_stop();
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_start();
}

static void verificar_y_levantar_mqtt(void) {
    if (!mqtt_started) {
        if (task_mqtt_start(NULL) == ESP_OK) {
            mqtt_started = true;
            logger_publicar_al_arrancar();
            ESP_LOGI(TAG, "MQTT iniciado exitosamente.");
        } else {
            ESP_LOGW(TAG, "Fallo al iniciar MQTT.");
        }
    }
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    ESP_LOGI(TAG, "EVENT: -> Entrando a wifi_event_handler. event_base: %s, event_id: %ld", event_base, event_id);

    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_DISCONNECTED: {
                ESP_LOGW(TAG, "EVENT: WIFI_EVENT_STA_DISCONNECTED");

                sta_connected = false;
                if (mqtt_started) {
                    task_mqtt_stop();
                    mqtt_started = false;
                    
                }

                wifi_event_sta_disconnected_t *disc = (wifi_event_sta_disconnected_t *)event_data;
                ESP_LOGW(TAG, "EVENT: STA desconectado. Motivo: %d", disc->reason);

                portENTER_CRITICAL(&state_mux);
                current_state = WIFI_STATE_WAITING_USER;
                portEXIT_CRITICAL(&state_mux);
                break;
            }

            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "EVENT: WIFI_EVENT_AP_START");
                break;

            case WIFI_EVENT_AP_STACONNECTED:
                ESP_LOGI(TAG, "EVENT: Dispositivo conectado al AP.");
                break;

            case WIFI_EVENT_AP_STADISCONNECTED:
                ESP_LOGI(TAG, "EVENT: Dispositivo desconectado del AP.");
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "EVENT: IP_EVENT_STA_GOT_IP");

        sta_connected = true;

        wifi_state_t estado_actual;
        portENTER_CRITICAL(&state_mux);
        estado_actual = current_state;
        portEXIT_CRITICAL(&state_mux);

        // Leer credenciales desde NVS
        wifi_credentials_t cred;
        bool tengo_cred = wifi_credentials_leer(&cred);

        // Consultar red a la que nos conectamos
        wifi_ap_record_t ap_info;
        esp_err_t r = esp_wifi_sta_get_ap_info(&ap_info);

        if (r == ESP_OK) {
            ESP_LOGI(TAG, "EVENT: STA obtuvo IP: " IPSTR, IP2STR(&((ip_event_got_ip_t *)event_data)->ip_info.ip));
            ESP_LOGI(TAG, "EVENT: Conectado al SSID: %s", ap_info.ssid);

            if (estado_actual == WIFI_STATE_WAITING_USER &&
                tengo_cred &&
                strcmp((char *)ap_info.ssid, cred.ssid) == 0) {

                ESP_LOGW(TAG, "EVENT: Red guardada detectada desde WAITING_USER. Volviendo a INIT.");
                portENTER_CRITICAL(&state_mux);
                current_state = WIFI_STATE_INIT;
                portEXIT_CRITICAL(&state_mux);

            }else if (estado_actual == WIFI_STATE_WAITING_USER) {
                ESP_LOGI(TAG, "EVENT: Conexión automática a red distinta: %s", ap_info.ssid);

            } else {
                portENTER_CRITICAL(&state_mux);
                current_state = WIFI_STATE_CONNECTED;
                portEXIT_CRITICAL(&state_mux);
            }
        }

        ntp_sync_inicializar();
        verificar_y_levantar_mqtt();
    }

    ESP_LOGI(TAG, "EVENT: <- Saliendo de wifi_event_handler");
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
        wifi_state_t estado;
        portENTER_CRITICAL(&state_mux);
        estado = current_state;
        portEXIT_CRITICAL(&state_mux);

        switch (estado) {
            case WIFI_STATE_INIT:
                ESP_LOGI(TAG, "FSM: -> Estado INIT");
                if (!wifi_credentials_leer(&cred) || strlen((char *)cred.ssid) == 0) {
                    portENTER_CRITICAL(&state_mux);
                    current_state = WIFI_STATE_WAIT_CREDENTIALS;
                    portEXIT_CRITICAL(&state_mux);
                } else {
                    portENTER_CRITICAL(&state_mux);
                    current_state = WIFI_STATE_CONNECTING;
                    portEXIT_CRITICAL(&state_mux);
                }
                ESP_LOGI(TAG, "FSM: <- Salida de INIT");
                break;

            case WIFI_STATE_WAIT_CREDENTIALS:
                ESP_LOGI(TAG, "FSM: -> Estado WAIT_CREDENTIALS");
                vTaskDelay(pdMS_TO_TICKS(10000));
                if (wifi_credentials_leer(&cred)) {
                    portENTER_CRITICAL(&state_mux);
                    current_state = WIFI_STATE_INIT;
                    portEXIT_CRITICAL(&state_mux);
                }
                ESP_LOGI(TAG, "FSM: <- Salida de WAIT_CREDENTIALS");
                break;

            case WIFI_STATE_CONNECTING:
                ESP_LOGI(TAG, "FSM: -> Estado CONNECTING");
                
                if (sta_connected) {
                    ESP_LOGW(TAG, "FSM: Ya estamos conectados. Saltando intento de conexión.");
                    portENTER_CRITICAL(&state_mux);
                    current_state = WIFI_STATE_CONNECTED;
                    portEXIT_CRITICAL(&state_mux);
                    break;
                }

                ESP_LOGI(TAG, "FSM: Intentando conectar a SSID: '%s'", cred.ssid);

                wifi_config_t sta_config = {0};
                strncpy((char *)sta_config.sta.ssid, cred.ssid, sizeof(sta_config.sta.ssid) - 1);
                strncpy((char *)sta_config.sta.password, cred.password, sizeof(sta_config.sta.password) - 1);
                sta_config.sta.threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK;
                wifi_mode_t modo_actual;

                ESP_ERROR_CHECK(esp_wifi_get_mode(&modo_actual));
                if (modo_actual != WIFI_MODE_APSTA) {
                    ESP_LOGW(TAG, "FSM: Cambiando a modo APSTA");
                    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
                }

                ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
                esp_wifi_connect();
                vTaskDelay(pdMS_TO_TICKS(10000));
                ESP_LOGI(TAG, "FSM: Llamada a esp_wifi_connect() realizada. Esperando conexión...");
                wifi_ap_record_t ap_info;
                if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
                    ESP_LOGI(TAG, "FSM: Conexión exitosa al AP: %s", ap_info.ssid);
                    portENTER_CRITICAL(&state_mux);
                    current_state = WIFI_STATE_CONNECTED;
                    portEXIT_CRITICAL(&state_mux);
                    reintentos = 0;
                } else {
                    reintentos++;
                    ESP_LOGW(TAG, "FSM: Fallo de conexión #%d", reintentos);
                    if (reintentos > MAX_REINTENTOS) {
                        ESP_LOGE(TAG, "FSM: Demasiados reintentos. Pasando a modo AP puro.");
                        wifi_modo_ap_puro();
                        portENTER_CRITICAL(&state_mux);
                        current_state = WIFI_STATE_WAITING_USER;
                        portEXIT_CRITICAL(&state_mux);
                        reintentos = 0;
                    }
                }
                ESP_LOGI(TAG, "FSM: <- Salida de CONNECTING");
                break;

            case WIFI_STATE_CONNECTED:
                ESP_LOGI(TAG, "FSM: -> Estado CONNECTED");

                EventBits_t bits = xEventGroupGetBits(wifi_event_group);
                if (bits & WIFI_EVENT_FLAG_NEW_CREDENTIALS) {
                    ESP_LOGI(TAG, "FSM: Nuevas credenciales detectadas en estado CONNECTED. Reiniciando conexión.");
                    xEventGroupClearBits(wifi_event_group, WIFI_EVENT_FLAG_NEW_CREDENTIALS);
                    esp_wifi_disconnect();  // ← corta la conexión actual
                    portENTER_CRITICAL(&state_mux);
                    current_state = WIFI_STATE_INIT;
                    portEXIT_CRITICAL(&state_mux);
                    ESP_LOGI(TAG, "FSM: <- Salida de CONNECTED, NUEVAS Credenciales");
                    break;
                }
                
                vTaskDelay(pdMS_TO_TICKS(1000));

                if (!sta_connected) {
                    ESP_LOGW(TAG, "FSM: Perdimos conexión inesperadamente. Reintentando.");
                    portENTER_CRITICAL(&state_mux);
                    current_state = WIFI_STATE_CONNECTING;
                    portEXIT_CRITICAL(&state_mux);
                }

                verificar_y_levantar_mqtt();
                ESP_LOGI(TAG, "FSM: <- Salida de CONNECTED");
                break;

            case WIFI_STATE_WAITING_USER:
                ESP_LOGI(TAG, "FSM: -> Estado WAITING_USER");

                EventBits_t wait_bits = xEventGroupWaitBits(wifi_event_group,
                                                    WIFI_EVENT_FLAG_NEW_CREDENTIALS,
                                                    pdTRUE, pdFALSE,
                                                    pdMS_TO_TICKS(5000));

                if (wait_bits & WIFI_EVENT_FLAG_NEW_CREDENTIALS) {
                    ESP_LOGI(TAG, "FSM: Evento de nuevas credenciales recibido");
                    portENTER_CRITICAL(&state_mux);
                    current_state = WIFI_STATE_INIT; 
                    portEXIT_CRITICAL(&state_mux);
                } else if (wifi_credentials_leer(&cred)) {
                    ESP_LOGI(TAG, "FSM: Credenciales válidas detectadas automáticamente. Volviendo a INIT.");
                    portENTER_CRITICAL(&state_mux);
                    current_state = WIFI_STATE_INIT;
                    portEXIT_CRITICAL(&state_mux);
                }

                ESP_LOGI(TAG, "FSM: <- Salida de WAITING_USER");
                break;

            default:
                ESP_LOGW(TAG, "FSM: Estado inválido, reiniciando a INIT");
                portENTER_CRITICAL(&state_mux);
                current_state = WIFI_STATE_INIT;
                portEXIT_CRITICAL(&state_mux);
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