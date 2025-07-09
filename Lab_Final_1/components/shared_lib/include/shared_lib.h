#ifndef SHARED_LIB_H_
#define SHARED_LIB_H_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "led_rgb.h"
#include "audio_player.h"

// --- Constantes
#define WIFI_NAMESPACE "wifi_config"
// --- Alias
#define wifi_credentials_cargar wifi_credentials_leer //Para que tenga un poco mas de sentido en el codigo de wifiAPSTA
// --- Estructuras
typedef struct {
    char ssid[24];
    char password[32];
} wifi_credentials_t;

// --- Variables compartidas
extern SemaphoreHandle_t color_semaphore;
extern SemaphoreHandle_t timer_done_semaphore;
extern led_rgb_evento_t current_color;
extern SemaphoreHandle_t i2c_mutex;
extern SemaphoreHandle_t io_mutex;
extern QueueHandle_t command_queue;
/// Mutex global para proteger acceso a SPIFFS
extern SemaphoreHandle_t spiffs_mutex;

extern QueueHandle_t audio_event_queue;


// Función de inicialización
void inicializar_recursos_globales(void);

// --- Funciones de credenciales WiFi
bool wifi_credentials_guardar(const wifi_credentials_t *cred);
bool wifi_credentials_leer(wifi_credentials_t *cred);
bool wifi_credentials_validas(const wifi_credentials_t *cred);
bool wifi_credentials_borrar(void);
esp_err_t mqtt_borrar_url(void);
// MONTAJE DE SPIFFS
esp_err_t spiffs_init(void);
#endif // SHARED_LIB_H_