#ifndef WEB_SERVICE_H
#define WEB_SERVICE_H

#include "esp_err.h"  // Para esp_err_t
#include "esp_http_server.h"

// Variable global visible externamente
extern int led_mode_web;

// Inicialización del servidor y ciclo de control
void web_service_inicializar(void);
void web_service_bucle(void);

// Handler HTTP para guardar credenciales desde el WebServer
esp_err_t guardar_wifi_handler(httpd_req_t *req);

// Guardado en NVS de las credenciales WiFi
bool nvs_guardar_credenciales(const char *ssid, const char *pass);

#endif // WEB_SERVICE_H