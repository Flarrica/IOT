#include "esp_sntp.h"
#include "esp_log.h"
#include "ntp.h"
#include <time.h>
#include <sys/time.h>

static const char *TAG = "NTP_SYNC";

void ntp_sync_inicializar(void)
{
    ESP_LOGI(TAG, "Inicializando SNTP");

    if (esp_sntp_enabled()) {
        ESP_LOGW("NTP_SYNC", "SNTP ya estaba iniciado. Reiniciando...");
        esp_sntp_stop();
    }
    
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    setenv("TZ", "UTC3", 1);  // Montevideo
    tzset();

    // Esperar hasta que se sincronice o agotar intentos
    const int MAX_RETRIES = 10;
    int retry = 0;
    time_t now;
    struct tm timeinfo;
    char buffer[64];

    while (retry < MAX_RETRIES) {
        time(&now);
        localtime_r(&now, &timeinfo);
        if (timeinfo.tm_year >= (2024 - 1900)) {
            strftime(buffer, sizeof(buffer), "%A %d de %B de %Y - %H:%M:%S", &timeinfo);
            ESP_LOGI(TAG, "SNTP sincronizado correctamente");
            ESP_LOGI(TAG, "Fecha y hora obtenida: %s", buffer);
            return;
        }
        ESP_LOGW(TAG, "Esperando sincronización NTP (%d/%d)...", retry + 1, MAX_RETRIES);
        vTaskDelay(pdMS_TO_TICKS(1000));
        retry++;
    }

    ESP_LOGE(TAG, "Fallo al sincronizar NTP tras %d intentos", MAX_RETRIES);
}