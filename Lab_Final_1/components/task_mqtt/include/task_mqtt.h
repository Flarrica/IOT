#ifndef TASK_MQTT_H
#define TASK_MQTT_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"

//Definir los tópicos
#define TOPIC_CONTROL "/control/reproduccion"
#define TOPIC_ESTADO "/estado/reproductor"
#define TOPIC_LOG   "/log/eventos"

// Estructura para el estado del reproductor
typedef struct {
    char estado[20];       // "reproduciendo", "pausado", "detenido"
    int volumen;            // 0-100
    char cancion[100];      // Nombre de la canción actual
} reproductor_estado_t;

// Prototipos de las funciones
esp_err_t task_mqtt_start(void *handler_args);
esp_err_t task_mqtt_publish_estado_reproductor(esp_mqtt_client_handle_t client, reproductor_estado_t estado);
void publicar_estado_reproductor(esp_mqtt_client_handle_t client, reproductor_estado_t estado);

#endif
