#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "esp_netif.h"
#include "esp_event.h"
#include "protocol_examples_common.h"

#include "task_a.h"
#include "task_b.h"
#include "task_c.h"
#include "led_rgb.h"
#include "shared_lib.h"
#include "task_mqtt.h"
#include "wifi_APSTA.h"
#include "web_service.h"

#include "nvs_flash.h"


void app_main(void) {
    // Info inicial del sistema (memoria libre, versión IDF)
    ESP_LOGI("MAIN", "[APP] Startup...");
    ESP_LOGI("MAIN", "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI("MAIN", "[APP] IDF version: %s", esp_get_idf_version());

    // Configuro los niveles de log deseados (para silenciar o mostrar cosas)
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("ws2812", ESP_LOG_WARN);   // LED RGB sin tanto ruido
    esp_log_level_set("LED_RGB", ESP_LOG_NONE);

    // Inicializamos NVS de forma segura (verifica si necesita formatear)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // Inicializamos la red y eventos de sistema
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Conectamos en modo AP+STA personalizado (SSID propio y conexión a red real)
    wifi_apsta_inicializar();  // Tu función propia (evita usar example_connect)

    // Inicializo el LED RGB WS2812
    led_rgb_inicializar();

    // Inicio el servidor web HTTP (para controlar el LED desde el navegador)
    ESP_LOGI("MAIN", "Inicializando servidor web HTTP...");
    web_service_inicializar();
    ESP_LOGI("MAIN", "Servidor HTTP. Ready! ");

    // Inicializo semáforos, colas y otros recursos compartidos
    ESP_LOGI("MAIN", "Semáforos, colas y otros recursos compartidos...");
    inicializar_recursos_globales();
    ESP_LOGI("MAIN", "Recursos compartidos. Ready!");
    
    
    // Inicializo MQTT y su tarea asociada (suscripción y publicación)
    ESP_LOGI("MAIN", "Inicializando MQTT y su tarea asociada...");
    int retries = 0; //Intentos de reconexion a la red WiFi
    while (!wifi_sta_conectado() && retries < 5) {
        ESP_LOGI("MAIN", "Esperando conexión WiFi... (%d)", retries);
        vTaskDelay(pdMS_TO_TICKS(500));
        retries++;
    }
    if (wifi_sta_conectado()) {
        iniciar_task_mqtt();
    } else {
        ESP_LOGW("MAIN", "No hay WiFi STA. MQTT no se iniciará.");
    }
    ESP_LOGI("MAIN", "MQTT. Ready!");
    // Lanzo tareas A, B y C (cada una maneja lógica distinta con eventos y timers)
    xTaskCreate(task_a, "task_a", 2048, NULL, 6, NULL);
    xTaskCreate(task_b, "task_b", 4096, NULL, 10, NULL);
    xTaskCreate(task_c, "task_c", 4096, NULL, 8, NULL);

    while (true) {
        // Procesa los botones web (desde HTML)
        web_service_bucle(); //Gastón: Esto debería pasar a ser una task

        // Pequeña pausa de control (10ms)
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
