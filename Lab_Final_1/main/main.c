#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "task_a.h"
#include "task_b.h"
#include "task_c.h"
#include "led_rgb.h"
#include "shared_lib.h"
#include "task_mqtt.h"
#include "wifi_APSTA.h"
#include "web_service.h"
#include "audio_player.h"
#include "touch_buttons.h"
#include "logger.h"

void app_main(void)
{
    // Info inicial del sistema
    ESP_LOGI("MAIN", "[APP] Startup...");
    ESP_LOGI("MAIN", "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI("MAIN", "[APP] IDF version: %s", esp_get_idf_version());

    // Configuro niveles de log
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("ws2812", ESP_LOG_WARN);
    esp_log_level_set("LED_RGB", ESP_LOG_NONE);
    
    ESP_LOGI("MAIN", "Semáforos, colas y recursos compartidos...");
    inicializar_recursos_globales();
    ESP_LOGI("MAIN", "Recursos compartidos. Ready!");
    vTaskDelay(pdMS_TO_TICKS(1000));
    // Creamos semaforos

    i2c_mutex = xSemaphoreCreateMutex();
    io_mutex = xSemaphoreCreateMutex();

    // Inicializar botones
    touch_polling_init();
    vTaskDelay(pdMS_TO_TICKS(1000));

    //Inicializamos AUDIO antes de WiFi para solucionar errores (audio no reproducía por conflicto con WiFi)
    
    
    ESP_LOGI("MAIN", "Inicializando audio...");
    ESP_LOGI("MAIN", "Inicializando audio...");
    if (audio_player_init() != ESP_OK) {
        ESP_LOGE("MAIN", "Fallo al inicializar audio");
    }
    
    // Delay para asegurar estabilidad antes del WiFi
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Inicialización de red y eventos
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // LED RGB
    led_rgb_inicializar();


    // Delay para asegurar estabilidad antes del WiFi
    vTaskDelay(pdMS_TO_TICKS(1000));

    // WiFi AP + STA
    wifi_apsta_inicializar();

    // Servidor web
    ESP_LOGI("MAIN", "Inicializando servidor web HTTP...");
    web_service_inicializar();
    ESP_LOGI("MAIN", "Servidor HTTP. Ready!");

    // Recursos compartidos
    
    
    
    // Intentamos conectar WiFi antes de iniciar MQTT
    
    ESP_LOGI("MAIN", "Inicializando MQTT y su tarea...");
    int retries = 0;
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
    
    // Lanzamos tareas
    
    vTaskDelay(pdMS_TO_TICKS(500));
    xTaskCreate(task_a, "task_a", 2048, NULL, 6, NULL);
    vTaskDelay(pdMS_TO_TICKS(500));
    xTaskCreate(task_b, "task_b", 4096, NULL, 10, NULL);
    vTaskDelay(pdMS_TO_TICKS(500));
    xTaskCreate(task_c, "task_c", 4096, NULL, 8, NULL);
    
    ESP_LOGI("MAIN", "Esperando para estabilizar touch...");
    vTaskDelay(pdMS_TO_TICKS(500));

    xTaskCreate(task_touch, "task_touch", 2048, NULL, 7, NULL);

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}