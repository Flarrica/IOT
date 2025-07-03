#include <stdio.h>

#include "esp_system.h"   // esp_restart
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_spiffs.h"

#include "nvs_flash.h"    // NVS init
#include "nvs.h"          // NVS read/write

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "web_service.h"

#include "led_rgb.h"
#include "audio_player.h"  // Asumiendo que ahí están los tipos y la cola

#define WIFI_NAMESPACE "wifi_config"

extern QueueHandle_t audio_cmd_queue;
static int ultimo_evento_web = LED_EVENT_INVALIDO;
static const char *TAG = "WEB_SERVER";
static httpd_handle_t server = NULL;

static esp_err_t root_handler(httpd_req_t *req) {
    const char *resp_str =
        "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
        "<meta charset=\"UTF-8\">"
        "<title>ESP32 Audio Player</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; background-color: #282c34; margin: 0; padding: 0;"
        "display: flex; flex-direction: column; justify-content: center; align-items: center; min-height: 100vh; color: #f0f0f0; }"
        ".titulo-principal { color: #61dafb; text-align: center; font-size: 2.5em; margin-top: 20px; }"
        ".subtitulo { color: #ffcc00; text-align: center; font-size: 1.5em; margin-top: 30px; margin-bottom: 10px; }"
        ".estado-reproductor { font-size: 1.2em; font-weight: bold; color: white; padding: 12px 20px; border-radius: 10px;"
        "margin-bottom: 20px; min-width: 160px; text-align: center; }"
        ".verde { background-color: #2ecc71; }"
        ".rojo { background-color: #e74c3c; }"
        ".azul { background-color: #3498db; }"
        ".amarillo { background-color: #f1c40f; color: #333; }"
        ".blanco { background-color: #ecf0f1; color: #333; }"
        ".cian { background-color: #1abc9c; }"
        ".button-container { display: flex; flex-wrap: wrap; justify-content: center; padding: 20px; gap: 12px; }"
        "button { width: 140px; height: 60px; font-size: 20px; margin: 8px; color: white; border: none; border-radius: 8px;"
        "cursor: pointer; transition: transform 0.2s, box-shadow 0.2s; display: flex; flex-direction: column; align-items: center; justify-content: center; }"
        "button:hover { transform: translateY(-3px); box-shadow: 0 6px 12px rgba(0, 0, 0, 0.4); }"
        "form { background-color: #3a3f47; padding: 20px; border-radius: 10px; max-width: 300px; width: 100%; margin-top: 20px; }"
        "input[type=text], input[type=password] { width: 100%; padding: 10px; margin: 8px 0; border-radius: 5px; border: none; font-size: 16px; }"
        "input[type=submit] { background-color: #1abc9c; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; }"
        "input[type=submit]:hover { background-color: #16a085; }"
        "</style></head><body>"

        "<h1 class=\"titulo-principal\">🎶 Control de Reproductor</h1>"

        // Indicador de estado actual del reproductor
        "<div id=\"estadoActual\" class=\"estado-reproductor rojo\">STOPPED</div>"

        "<div class=\"button-container\">"
        "<button class=\"blanco\" onclick=\"sendCmd('prev')\">⏮<br>Prev</button>"
        "<button class=\"verde\" onclick=\"sendCmd('play')\">▶<br>Play</button>"
        "<button class=\"azul\" onclick=\"sendCmd('pause')\">⏸<br>Pause</button>"
        "<button class=\"rojo\" onclick=\"sendCmd('stop')\">⏹<br>Stop</button>"
        "<button class=\"amarillo\" onclick=\"sendCmd('next')\">⏭<br>Next</button>"
        "<button class=\"cian\" onclick=\"sendCmd('volup')\">🔊<br>Vol +</button>"
        "<button class=\"cian\" onclick=\"sendCmd('voldown')\">🔉<br>Vol -</button>"
        "</div>"

        "<h2 class=\"subtitulo\">📶 Configuración WiFi</h2>"
        "<form action=\"/guardar_wifi\" method=\"post\">"
        "<label for=\"ssid\">SSID:</label><br>"
        "<input type=\"text\" id=\"ssid\" name=\"ssid\" required><br>"
        "<label for=\"pass\">Contraseña:</label><br>"
        "<input type=\"password\" id=\"pass\" name=\"pass\" required><br><br>"
        "<input type=\"submit\" value=\"Guardar y Conectar\">"
        "</form>"

        "<script>"
        "function sendCmd(cmd) {"
        "  fetch('/cmd?cmd=' + cmd)"
        "    .then(response => response.ok && console.log('Comando enviado:', cmd))"
        "    .catch(error => console.error('Error en fetch:', error));"
        "}"
        ""
        "function actualizarEstado() {"
        "  fetch('/status')"
        "    .then(res => res.json())"
        "    .then(data => {"
        "      const estado = data.estado;"
        "      const div = document.getElementById('estadoActual');"
        "      div.textContent = estado.toUpperCase();"
        "      div.classList.remove('verde', 'rojo', 'azul');"
        "      if (estado === 'playing') div.classList.add('verde');"
        "      else if (estado === 'paused') div.classList.add('azul');"
        "      else div.classList.add('rojo');"
        "    })"
        "    .catch(err => console.error('Error al obtener estado:', err));"
        "}"
        "setInterval(actualizarEstado, 2000);"
        "window.onload = actualizarEstado;"
        "</script></body></html>";

    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t audio_status_handler(httpd_req_t *req) {
    const char *estado_str;
    audio_state_t estado = audio_player_get_state();  // ✅ Consulta segura

    switch (estado) {
        case PLAYER_PLAYING: estado_str = "playing"; break;
        case PLAYER_PAUSED:  estado_str = "paused";  break;
        case PLAYER_STOPPED: estado_str = "stopped"; break;
        default:             estado_str = "unknown"; break;
    }

    httpd_resp_set_type(req, "application/json");

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "{\"estado\":\"%s\"}", estado_str);

    return httpd_resp_sendstr(req, buffer);
}

static esp_err_t audio_cmd_handler(httpd_req_t *req) {
    char buf[64];
    size_t len = httpd_req_get_url_query_len(req) + 1;

    if (len > 1 && httpd_req_get_url_query_str(req, buf, len) == ESP_OK) {
        char cmd_str[32];
        if (httpd_query_key_value(buf, "cmd", cmd_str, sizeof(cmd_str)) == ESP_OK) {
            audio_cmd_t cmd;

            if      (strcmp(cmd_str, "play")    == 0) cmd = CMD_PLAY;
            else if (strcmp(cmd_str, "pause")   == 0) cmd = CMD_PAUSE;
            else if (strcmp(cmd_str, "stop")    == 0) cmd = CMD_STOP;
            else if (strcmp(cmd_str, "next")    == 0) cmd = CMD_NEXT;
            else if (strcmp(cmd_str, "prev")    == 0) cmd = CMD_PREV;
            else if (strcmp(cmd_str, "volup")   == 0) cmd = CMD_VOL_UP;
            else if (strcmp(cmd_str, "voldown") == 0) cmd = CMD_VOL_DOWN;
            else {
                httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Comando inválido");
                return ESP_FAIL;
            }

            audio_player_send_cmd(cmd);
            return httpd_resp_sendstr(req, "OK");
        } else {
            return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Parámetro 'cmd' faltante");
        }
    }

    return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Faltan parámetros");
}

static esp_err_t favicon_handler(httpd_req_t *req) {
    // Podés enviar un ícono vacío, o solo un 204 sin contenido
    httpd_resp_send(req, NULL, 0);  // HTTP 200 sin cuerpo
    return ESP_OK;
}

void web_service_inicializar(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_LOGI(TAG, "Iniciando WebServer");
    config.max_uri_handlers = 8;
    config.recv_wait_timeout = 10;
    config.lru_purge_enable = true;

    // Tuvimos problemas de header asi que aumentamos tamaños
    config.stack_size = 8192;            // Aumentamos stack por seguridad

    ESP_LOGI(TAG, "Iniciando WebServer");
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t uri_root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_root);

        httpd_uri_t uri_wifi = {
            .uri = "/guardar_wifi",
            .method = HTTP_POST,
            .handler = guardar_wifi_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_wifi);

        httpd_uri_t estado_uri = {
            .uri       = "/status",
            .method    = HTTP_GET,
            .handler   = audio_status_handler,
            .user_ctx  = NULL
        };

        httpd_register_uri_handler(server, &estado_uri);

        httpd_uri_t comando_audio_uri = {
            .uri      = "/cmd",
            .method   = HTTP_GET,
            .handler  = audio_cmd_handler,
            .user_ctx = NULL
        };

        httpd_register_uri_handler(server, &comando_audio_uri);
        httpd_uri_t favicon_uri = {
            .uri      = "/favicon.ico",
            .method   = HTTP_GET,
            .handler  = favicon_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &favicon_uri);
    }
}

esp_err_t guardar_wifi_handler(httpd_req_t *req) {
    char buf[256];
    int ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret <= 0) return ESP_FAIL;

    // Extraer ssid y pass del cuerpo (muy básico, mejorar para casos reales)
    char ssid[32] = {0};
    char pass[64] = {0};
    sscanf(buf, "ssid=%31[^&]&pass=%63s", ssid, pass);

    ESP_LOGI(TAG, "Recibido: ssid=%s, pass=%s", ssid, pass);

    if (nvs_guardar_credenciales(ssid, pass)) {
        const char *html_resp = 
            "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>WiFi Guardado</title></head>"
            "<body style='font-family:Arial; background:#282c34; color:#f0f0f0; text-align:center; padding-top:20%%;'>"
            "<h1>✅ Credenciales guardadas</h1>"
            "<p>El dispositivo se reiniciará en breve para conectarse a la red...</p>"
            "</body></html>";

        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, html_resp, HTTPD_RESP_USE_STRLEN);

        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_restart(); // Reinicia para que se conecte automáticamente
    } else {
        httpd_resp_sendstr(req, "Error al guardar credenciales.");
    }

    return ESP_OK;
}

bool nvs_guardar_credenciales(const char *ssid, const char *password) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(WIFI_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) return false;

    err = nvs_set_str(nvs_handle, "ssid", ssid);
    if (err != ESP_OK) {
        nvs_close(nvs_handle);
        return false;
    }

    err = nvs_set_str(nvs_handle, "pass", password);
    if (err != ESP_OK) {
        nvs_close(nvs_handle);
        return false;
    }

    err = nvs_commit(nvs_handle); // Graba en flash
    nvs_close(nvs_handle);

    return err == ESP_OK;
}