#ifndef WIFI_APSTA_H
#define WIFI_APSTA_H

#include <stdbool.h>
#include <stdint.h>             // Para tipos uint32_t, etc.
#include "freertos/FreeRTOS.h" // Para tipos de FreeRTOS
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"          // Para estructuras WiFi (wifi_config_t, etc.)
#include "esp_log.h"

//------------------------------
// Definición de estados de la FSM WiFi
//------------------------------
typedef enum {
    WIFI_STATE_INIT,              // Inicio del sistema
    WIFI_STATE_WAIT_CREDENTIALS, // Esperando credenciales en NVS
    WIFI_STATE_CONNECTING,       // Intentando conectar como STA
    WIFI_STATE_CONNECTED,        // Conexión exitosa como STA
    WIFI_STATE_WAITING_USER      // Esperando nuevas credenciales vía Web
} wifi_state_t;

//------------------------------
// Inicializa la FSM de WiFi:
// - Crea el EventGroup
// - Inicia la tarea FSM
// - Configura WiFi en modo AP o AP+STA según el caso
//------------------------------
void wifi_fsm_inicializar(void);

//------------------------------
// Tarea principal de la FSM WiFi
// Ejecuta la lógica de conexión, reconexión y fallback a AP
//------------------------------
void wifi_fsm_task(void *param);

//------------------------------
// Notifica a la FSM que hay nuevas credenciales disponibles
// (ej: luego de que el usuario las envía por Web)
//------------------------------
void wifi_fsm_notificar_nuevas_credenciales(void);

//------------------------------
// Fuerza el sistema a modo AP puro
// - Detiene STA
// - Reinicia en modo AP sin intentar reconectar
//------------------------------
void wifi_modo_ap_puro(void);

//------------------------------
// Inicializa la configuración AP+STA (modo dual) de WiFi
// - Se utiliza desde FSM o durante boot según estrategia
//------------------------------
void wifi_apsta_inicializar(void);

#endif  // WIFI_APSTA_H
