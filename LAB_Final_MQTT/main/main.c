#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "esp_netif.h"
#include "esp_event.h"

#include "task_a.h"
#include "task_b.h"
#include "task_c.h"
#include "led_rgb.h"
#include "shared_lib.h"
#include "task_mqtt.h"
#include "wifi_APSTA.h"
#include "web_service.h"
#include "audio_player.h"

#include "nvs_flash.h"

// Para ir al menuconfig
// cd C:\Users\gasto\esp\v5.4.1\esp-idf
// .\export.ps1
// idf.py menuconfig

void app_main(void) {
    // Info inicial del sistema (memoria libre, versión IDF)
    ESP_LOGI("MAIN", "[APP] Startup...");
    ESP_LOGI("MAIN", "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI("MAIN", "[APP] IDF version: %s", esp_get_idf_version());

    // Configuro niveles de log deseados
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("ws2812", ESP_LOG_WARN);
    esp_log_level_set("LED_RGB", ESP_LOG_NONE);

    // Inicializamos NVS (flash de sistema)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // Inicializamos sistema de red y bucle de eventos
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Montamos SPIFFS (sistema de archivos)
    ESP_ERROR_CHECK(init_spiffs());

    // Inicializamos LED RGB
    led_rgb_inicializar();

    // Configuramos WiFi AP+STA
    wifi_apsta_inicializar();

    // Iniciamos servidor web
    ESP_LOGI("MAIN", "Inicializando servidor web HTTP...");
    web_service_inicializar();
    ESP_LOGI("MAIN", "Servidor HTTP. Ready!");

    // Inicializamos semáforos, colas y recursos compartidos
    ESP_LOGI("MAIN", "Semáforos, colas y otros recursos compartidos...");
    inicializar_recursos_globales();
    ESP_LOGI("MAIN", "Recursos compartidos. Ready!");

    // Intentamos conectarnos a red WiFi antes de iniciar MQTT
    ESP_LOGI("MAIN", "Inicializando MQTT y su tarea asociada...");
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
    ESP_LOGI("MAIN", "MQTT. Ready!");

    //  inicializamos el audio (I2S + ES8311) y creamos la task
    ESP_ERROR_CHECK(audio_player_init());
    xTaskCreate(task_audio_player, "task_audio", 4096, NULL, 5, NULL);

    // Lanzamos otras tareas de aplicación
    xTaskCreate(task_a, "task_a", 2048, NULL, 6, NULL);
    xTaskCreate(task_b, "task_b", 4096, NULL, 10, NULL);
    xTaskCreate(task_c, "task_c", 4096, NULL, 8, NULL);
    
    // Esperamos a que task_audio_player cree su cola
    vTaskDelay(pdMS_TO_TICKS(300));

    // Enviamos el primer comando de reproducción automática
    audio_event_t evt = { .cmd = AUDIO_CMD_PLAY };
    audio_player_enqueue(evt);

    // Bucle principal del servidor web
    while (true) {
        web_service_bucle();  // Recomendado mover a su propia tarea
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
