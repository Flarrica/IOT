#include <string.h>
#include <stdbool.h>
#include "wifi_APSTA.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_system.h"

#include "task_mqtt.h"
#include "task_a.h"
#include "task_b.h"
#include "task_c.h"
#include "led_rgb.h"
#include "shared_lib.h"
#include "web_service.h"
#include "audio_player.h"
#include "touch_buttons.h"

static const char *TAG = "MAIN";
void wait_for_wifi(void);
void app_main(void) {
    // Información inicial del sistema
    ESP_LOGI("MAIN", "[APP] Startup...");
    ESP_LOGI("MAIN", "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI("MAIN", "[APP] IDF version: %s", esp_get_idf_version());

    // Configuración de niveles de log
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("ws2812", ESP_LOG_WARN);
    esp_log_level_set("LED_RGB", ESP_LOG_NONE);

    // Crear semáforos
    i2c_mutex = xSemaphoreCreateMutex();
    io_mutex = xSemaphoreCreateMutex();

    // Inicializar botones
    touch_polling_init();
    vTaskDelay(pdMS_TO_TICKS(1000));

    //##################################
    // Inicializar Wi-Fi (tú tienes esa función)
    ESP_LOGI("MAIN", "Iniciando Wi-Fi...");

    wifi_apsta_inicializar();      
     // Esperar a que la Wi-Fi esté conectada antes de seguir
     wait_for_wifi();
    //wifi_apsta_bucle();

    // Inicializar audio (antes de WiFi)
    ESP_LOGI("MAIN","Wifi conectado, Inicializando audio...");
    if (audio_player_init() != ESP_OK){
        ESP_LOGE("MAIN", "Fallo al inicializar audio");
    }
    
    //##################################
    vTaskDelay(pdMS_TO_TICKS(1000));

    // La inicialización de red y eventos en tu código anterior estaba comentada
    // Ahora la haremos aquí.

    

    // Recursos compartidos
    ESP_LOGI("MAIN", "Semáforos, colas y recursos compartidos...");
    inicializar_recursos_globales();
    ESP_LOGI("MAIN", "Recursos compartidos. Listo!");



    // Ahora sí, iniciar la tarea MQTT
    ESP_LOGI("MAIN", "Wi-Fi lista, iniciando MQTT...");
    ESP_LOGI(TAG, "task_mqtt_start: Calling");
    ESP_ERROR_CHECK(task_mqtt_start(NULL));
    ESP_LOGI(TAG, "task_mqtt_start: Done");


    // Lanzar tareas adicionales, si tienes
    vTaskDelay(pdMS_TO_TICKS(500));
    xTaskCreate(task_a, "task_a", 2048, NULL, 6, NULL);
    vTaskDelay(pdMS_TO_TICKS(500));
    xTaskCreate(task_b, "task_b", 4096, NULL, 10, NULL);
    vTaskDelay(pdMS_TO_TICKS(500));
    xTaskCreate(task_c, "task_c", 4096, NULL, 8, NULL);

    ESP_LOGI("MAIN", "Esperando para estabilizar touch...");
    vTaskDelay(pdMS_TO_TICKS(500));

    xTaskCreate(task_touch, "task_touch", 2048, NULL, 7, NULL);

    // Bucle principal
    while (true) {
        //web_service_bucle();  // si tienes habilitado
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
