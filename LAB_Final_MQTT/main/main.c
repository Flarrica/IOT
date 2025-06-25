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
#include "nvs_flash.h"


void app_main(void) {
    ESP_LOGI("MAIN", "[APP] Startup...");
    ESP_LOGI("MAIN", "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI("MAIN", "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("ws2812", ESP_LOG_WARN);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());  // Asegúrate de tener esta función disponible

    led_rgb_inicializar();
    inicializar_recursos_globales();

    xTaskCreate(task_a, "task_a", 2048, NULL, 6, NULL); 
    xTaskCreate(task_b, "task_b", 4096, NULL, 10, NULL);
    xTaskCreate(task_c, "task_c", 4096, NULL, 8, NULL);

    iniciar_task_mqtt();
}
