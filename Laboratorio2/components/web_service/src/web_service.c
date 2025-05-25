#include "web_service.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "led_rgb.h"


static const char *TAG = "WEB_SERVER";
static httpd_handle_t server = NULL;
int led_mode_web = -1;  // o cualquier valor por defecto

static esp_err_t root_handler(httpd_req_t *req) {
    const char *resp_str =
        "<!DOCTYPE html><html><head><title>ESP32 LED</title>"
        "<style>"
        "button {"
        "  width: 100px; height: 60px; font-size: 18px; margin: 5px; color: white; border: none; border-radius: 8px;"
        "}"
        ".rojo { background-color: red; }"
        ".verde { background-color: green; }"
        ".azul { background-color: blue; }"
        ".blanco { background-color: white; color: black; }"
        ".amarillo { background-color: yellow; color: black; }"
        ".cian { background-color: cyan; color: black; }"
        ".off { background-color: gray; }"
        "</style></head><body>"
        "<h2>Control de LED desde Web</h2>"
        "<button class=\"rojo\" onclick=\"sendColor('rojo')\">Rojo</button>"
        "<button class=\"verde\" onclick=\"sendColor('verde')\">Verde</button>"
        "<button class=\"azul\" onclick=\"sendColor('azul')\">Azul</button>"
        "<button class=\"blanco\" onclick=\"sendColor('blanco')\">Blanco</button>"
        "<button class=\"amarillo\" onclick=\"sendColor('amarillo')\">Amarillo</button>"
        "<button class=\"cian\" onclick=\"sendColor('cian')\">Cian</button>"
        "<button class=\"off\" onclick=\"sendColor('off')\">Apagar</button>"
        "<script>"
        "function sendColor(color) {"
        "  fetch('/led?color=' + color)"
        "    .then(response => console.log('Color enviado:', color));"
        "}"
        "</script></body></html>";
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t led_handler(httpd_req_t *req) {
    char buf[100];
    size_t len = httpd_req_get_url_query_len(req) + 1;

    if (len > 1) {
        httpd_req_get_url_query_str(req, buf, len);
        char param[32];
        if (httpd_query_key_value(buf, "color", param, sizeof(param)) == ESP_OK) {
            if (strcmp(param, "rojo") == 0)
                led_mode_web = LED_EVENT_ROJO;
            else if (strcmp(param, "verde") == 0)
                led_mode_web = LED_EVENT_VERDE;
            else if (strcmp(param, "azul") == 0)
                led_mode_web = LED_EVENT_AZUL;
            else if (strcmp(param, "blanco") == 0)
                led_mode_web = LED_EVENT_BLANCO;
            else if (strcmp(param, "amarillo") == 0)
                led_mode_web = LED_EVENT_AMARILLO;
            else if (strcmp(param, "cian") == 0)
                led_mode_web = LED_EVENT_CIAN;
            else if (strcmp(param, "off") == 0)
                led_mode_web = LED_EVENT_APAGAR;
        }
    }
    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void web_service_inicializar(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_LOGI(TAG, "Iniciando WebServer");

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t uri_root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_root);

        httpd_uri_t uri_led = {
            .uri = "/led",
            .method = HTTP_GET,
            .handler = led_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_led);
    }
}

void web_service_bucle(void) {
    // No hace nada, el servidor corre en una tarea propia de IDF
    // Queda por las dudas que lo precisemos para algo
}