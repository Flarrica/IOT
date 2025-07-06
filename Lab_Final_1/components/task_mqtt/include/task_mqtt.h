#ifndef TASK_MQTT_H
#define TASK_MQTT_H

#include "esp_err.h"
#include "mqtt_client.h"

// Tópicos MQTT
#define TOPIC_CONTROL "/placaKaluga/mejorGrupo/nintendo/control"
#define TOPIC_ESTADO  "/placaKaluga/mejorGrupo/nintendo/estado"
#define TOPIC_LOG     "/placaKaluga/mejorGrupo/nintendo/log"
#define TOPIC_ENERGIA "/placaKaluga/mejorGrupo/nintendo/energia"

// Estructura del estado del reproductor
typedef struct {
    char estado[32];
    int volumen;
    char cancion[64];
} reproductor_estado_t;

// Estado global del reproductor
extern reproductor_estado_t estado_reproductor;

// Funciones públicas
esp_err_t task_mqtt_start(void *handler_args);
esp_err_t mqtt_guardar_url(const char *url);
esp_err_t mqtt_leer_url(char *dest, size_t max_len);
void publicar_estado_reproductor(esp_mqtt_client_handle_t client, reproductor_estado_t estado);
char* crear_evento_energia();

#endif // TASK_MQTT_H