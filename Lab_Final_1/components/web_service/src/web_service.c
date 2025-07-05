#include <stdio.h>
#include <ctype.h>

#include "esp_system.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "web_service.h"
#include "led_rgb.h"
#include "audio_player.h"

#define WIFI_NAMESPACE "wifi_config"

extern QueueHandle_t audio_cmd_queue;
static const char *TAG = "WEB_SERVER";
static httpd_handle_t server = NULL;

void url_decode(char *dst, const char *src, size_t dst_len) {
    char a, b;
    size_t i = 0;
    while (*src && i + 1 < dst_len) {
        if (*src == '%') {
            if ((a = src[1]) && (b = src[2]) && isxdigit(a) && isxdigit(b)) {
                a = (a <= '9') ? a - '0' : tolower(a) - 'a' + 10;
                b = (b <= '9') ? b - '0' : tolower(b) - 'a' + 10;
                dst[i++] = (char)(16 * a + b);
                src += 3;
            } else {
                break;
            }
        } else if (*src == '+') {
            dst[i++] = ' ';
            src++;
        } else {
            dst[i++] = *src++;
        }
    }
    dst[i] = '\0';
}

static esp_err_t root_handler(httpd_req_t *req) {
    const char *resp_str =
        "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
        "<meta charset=\"UTF-8\">"
        "<title>ESP32 Audio Player</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; background-color: #282c34; color: #f0f0f0; display: flex; flex-direction: column; align-items: center; justify-content: center; min-height: 100vh; margin: 0; }"
        "h1, h2 { text-align: center; }"
        ".button-container { display: flex; flex-wrap: wrap; justify-content: center; gap: 12px; padding: 20px; }"
        "button { width: 140px; height: 60px; font-size: 20px; margin: 8px; color: white; border: none; border-radius: 8px; cursor: pointer; transition: transform 0.2s, box-shadow 0.2s; display: flex; flex-direction: column; align-items: center; justify-content: center; }"
        "button:hover { transform: translateY(-3px); box-shadow: 0 6px 12px rgba(0, 0, 0, 0.4); }"
        ".estado-reproductor { font-size: 1.2em; font-weight: bold; padding: 12px 20px; border-radius: 10px; margin-bottom: 20px; min-width: 160px; text-align: center; }"
        ".verde { background-color: #2ecc71; } .rojo { background-color: #e74c3c; } .azul { background-color: #3498db; }"
        ".amarillo { background-color: #f1c40f; color: #333; } .blanco { background-color: #ecf0f1; color: #333; } .cian { background-color: #1abc9c; }"
        "form { background-color: #3a3f47; padding: 20px; border-radius: 10px; max-width: 300px; width: 100%; margin: 10px; }"
        "input[type=text], input[type=password], input[type=submit] { width: 100%; padding: 10px; margin: 8px 0; border-radius: 5px; border: none; font-size: 16px; }"
        "input[type=submit] { background-color: #1abc9c; color: white; cursor: pointer; }"
        "input[type=submit]:hover { background-color: #16a085; }"
        "</style></head><body>"
        "<h1>🎶 Control de Reproductor</h1>"
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
        "<h2>📶 Configuración WiFi</h2>"
        "<form action=\"/guardar_ssid\" method=\"post\">"
        "<label for=\"ssid\">SSID:</label><br>"
        "<input type=\"text\" id=\"ssid\" name=\"ssid\" required><br>"
        "<input type=\"submit\" value=\"Guardar SSID\">"
        "</form>"
        "<form action=\"/guardar_pass\" method=\"post\">"
        "<label for=\"pass\">Contraseña:</label><br>"
        "<input type=\"password\" id=\"pass\" name=\"pass\" required><br>"
        "<input type=\"submit\" value=\"Guardar Contraseña\">"
        "</form>"
        "<script>"
        "function sendCmd(cmd) { fetch('/cmd?cmd=' + cmd); }"
        "function actualizarEstado() {"
        "  fetch('/status').then(res => res.json()).then(data => {"
        "    const estado = data.estado;"
        "    const div = document.getElementById('estadoActual');"
        "    div.textContent = estado.toUpperCase();"
        "    div.className = 'estado-reproductor ' + (estado === 'playing' ? 'verde' : estado === 'paused' ? 'azul' : 'rojo');"
        "  });"
        "}"
        "setInterval(actualizarEstado, 2000);"
        "window.onload = actualizarEstado;"
        "</script></body></html>";

    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t audio_status_handler(httpd_req_t *req) {
    const char *estado_str;
    audio_state_t estado = audio_player_get_state();
    switch (estado) {
        case PLAYER_PLAYING: estado_str = "playing"; break;
        case PLAYER_PAUSED:  estado_str = "paused"; break;
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
            else return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Comando inválido");
            audio_player_send_cmd(cmd);
            return httpd_resp_sendstr(req, "OK");
        }
    }
    return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Faltan parámetros");
}

static esp_err_t guardar_ssid_handler(httpd_req_t *req) {
    char buf[128] = {0};
    if (httpd_req_recv(req, buf, sizeof(buf)) <= 0) return ESP_FAIL;
    char ssid[64] = {0};
    char *ssid_start = strstr(buf, "ssid=");
    if (ssid_start) {
        ssid_start += 5;
        url_decode(ssid, ssid_start, sizeof(ssid));
    }
    ESP_LOGI(TAG, "Recibido solo SSID: '%s'", ssid);
    nvs_handle_t nvs_handle;
    if (nvs_open(WIFI_NAMESPACE, NVS_READWRITE, &nvs_handle) == ESP_OK) {
        nvs_set_str(nvs_handle, "ssid", ssid);
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
        return httpd_resp_sendstr(req, "✅ SSID guardado");
    }
    return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "❌ Error guardando SSID");
}

static esp_err_t guardar_pass_handler(httpd_req_t *req) {
    char buf[128] = {0};
    if (httpd_req_recv(req, buf, sizeof(buf)) <= 0) return ESP_FAIL;
    char pass[64] = {0};
    char *pass_start = strstr(buf, "pass=");
    if (pass_start) {
        pass_start += 5;
        url_decode(pass, pass_start, sizeof(pass));
    }
    ESP_LOGI(TAG, "Recibido solo PASS: '%s'", pass);
    nvs_handle_t nvs_handle;
    if (nvs_open(WIFI_NAMESPACE, NVS_READWRITE, &nvs_handle) == ESP_OK) {
        nvs_set_str(nvs_handle, "pass", pass);
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
        return httpd_resp_sendstr(req, "✅ Contraseña guardada");
    }
    return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "❌ Error guardando PASS");
}

void web_service_inicializar(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 8;
    config.stack_size = 8192;
    config.recv_wait_timeout = 10;
    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Iniciando WebServer");
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &(httpd_uri_t){"/", HTTP_GET, root_handler, NULL});
        httpd_register_uri_handler(server, &(httpd_uri_t){"/status", HTTP_GET, audio_status_handler, NULL});
        httpd_register_uri_handler(server, &(httpd_uri_t){"/cmd", HTTP_GET, audio_cmd_handler, NULL});
        httpd_register_uri_handler(server, &(httpd_uri_t){"/guardar_ssid", HTTP_POST, guardar_ssid_handler, NULL});
        httpd_register_uri_handler(server, &(httpd_uri_t){"/guardar_pass", HTTP_POST, guardar_pass_handler, NULL});
    }
}