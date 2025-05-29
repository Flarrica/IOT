#include "web_service.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "led_rgb.h"

static int ultimo_evento_web = LED_EVENT_INVALIDO;
static const char *TAG = "WEB_SERVER";
static httpd_handle_t server = NULL;

static esp_err_t root_handler(httpd_req_t *req) {
    const char *resp_str =
        "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" // Asegura que se vea bien en móviles
        "<title>ESP32 LED Control</title>" // Título de la pestaña del navegador
        "<style>"
        "body {"
        "  font-family: Arial, sans-serif;"
        "  background-color: #282c34;" /* Color de fondo oscuro y elegante */
        "  margin: 0; padding: 0;"
        "  display: flex;"
        "  flex-direction: column;" /* Organiza los elementos en columna */
        "  justify-content: center;"/* Centra verticalmente */
        "  align-items: center;" /* Centra horizontalmente */
        "  min-height: 100vh;" /* Ocupa al menos el 100% de la altura de la ventana */
        "  color: #f0f0f0; " /* Color de texto general para la página */
        "}"
        ".titulo-principal {"
        "  color: #61dafb; "/* Azul claro para el título principal */
        "  text-align: center;"
        "  font-size: 2.5em; "/* Tamaño de fuente */
        "  margin-bottom: 10px;"
        "}"
        ".subtitulo {"
        "  color: #ffcc00; "/* Amarillo/naranja para el subtítulo */
        "  text-align: center;"
        "  font-size: 1.8em;"
        "  margin-bottom: 20px;"
        "}"
        ".button-container {"
        "  display: flex;"
        "  justify-content: center;"
        "  align-items: center;"
        "  flex-wrap: wrap;" /* Permite que los botones se envuelvan a la siguiente línea */
        "  max-width: 800px; "/* Ancho máximo para el contenedor de botones */
        "  padding: 10px;"
        "  border-radius: 10px;"
        "  background-color: #3a3f47;" /* Fondo sutil para el contenedor de botones */
        "}"
        "button {"
        "  width: 120px; height: 70px;"
        "  font-size: 18px; margin: 8px;" /* Margen y fuente */
        "  color: white; border: none; border-radius: 8px;"
        "  cursor: pointer;" /* Indica que es clickeable */
        "  transition: transform 0.2s, box-shadow 0.2s; "/* Animación al pasar el mouse */
        "  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.3); " /* Sombra para efecto 3D */
        "}"
        "button:hover {"
        "  transform: translateY(-3px); " /* Levanta el botón al pasar el mouse */
        "  box-shadow: 0 6px 12px rgba(0, 0, 0, 0.4);"
        "}"
        ".rojo { background-color: #e74c3c; }" /* Colores más agradables */
        ".verde { background-color: #2ecc71; }"
        ".azul { background-color: #3498db; }"
        ".blanco { background-color: #ecf0f1; color: #333; }"
        ".amarillo { background-color: #f1c40f; color: #333; }"
        ".cian { background-color: #1abc9c; }"
        ".off { background-color: #7f8c8d; }"
        "</style></head><body>"
         "<h1 class=\"titulo-principal\">ESP32 Control de LED</h1>"
        "<h2 class=\"subtitulo\">Control de LED desde Web</h2>"
        "<div class=\"button-container\">"
        "<button class=\"rojo\" onclick=\"sendColor('rojo')\">Rojo</button>"
        "<button class=\"verde\" onclick=\"sendColor('verde')\">Verde</button>"
        "<button class=\"azul\" onclick=\"sendColor('azul')\">Azul</button>"
        "<button class=\"blanco\" onclick=\"sendColor('blanco')\">Blanco</button>"
        "<button class=\"amarillo\" onclick=\"sendColor('amarillo')\">Amarillo</button>"
        "<button class=\"cian\" onclick=\"sendColor('cian')\">Cian</button>"
        "<button class=\"off\" onclick=\"sendColor('off')\">Apagar</button>"
        "</div>" // Cierre del button-container
        "<script>"
        "function sendColor(color) {"
        "  fetch('/led?color=' + color)"
        "    .then(response => {"
        "      if (response.ok) {"
        "        console.log('Color enviado:', color);"
        "      } else {"
        "        console.error('Error al enviar color:', response.statusText);"
        "      }"
        "    })"
        "    .catch(error => console.error('Error en la solicitud fetch:', error));"
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
                ultimo_evento_web = LED_EVENT_ROJO;
            else if (strcmp(param, "verde") == 0)
                ultimo_evento_web = LED_EVENT_VERDE;
            else if (strcmp(param, "azul") == 0)
                ultimo_evento_web = LED_EVENT_AZUL;
            else if (strcmp(param, "blanco") == 0)
                ultimo_evento_web = LED_EVENT_BLANCO;
            else if (strcmp(param, "amarillo") == 0)
                ultimo_evento_web = LED_EVENT_AMARILLO;
            else if (strcmp(param, "cian") == 0)
                ultimo_evento_web = LED_EVENT_CIAN;
            else if (strcmp(param, "off") == 0)
                ultimo_evento_web = LED_EVENT_APAGAR;
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
    static int evento_previo = LED_EVENT_INVALIDO;

    if (ultimo_evento_web != LED_EVENT_INVALIDO &&
        ultimo_evento_web != evento_previo) {
        led_rgb_set_event(ultimo_evento_web);
        evento_previo = ultimo_evento_web;
    }
}