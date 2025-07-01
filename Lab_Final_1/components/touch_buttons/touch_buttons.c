#include <inttypes.h>
#include "touch_buttons.h"

#include "driver/touch_pad.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led_rgb.h"
#include "audio_player.h"
#include "shared_lib.h"

#define TAG "TOUCH_BUTTONS"
#define TOUCH_BUTTON_NUM 6
#define TOUCH_THRESHOLD 60000

#define BENCHMARK_INVALID_MAX 4194300
#define BENCHMARK_MIN_VALID   1000
#define THRESHOLD_RATIO       1.20f  // Umbral menos agresivo, más sensible

// Parámetros configurables para denoise
#define TOUCH_DENOISE_GRADE       TOUCH_PAD_DENOISE_BIT4
#define TOUCH_DENOISE_CAP_LEVEL   TOUCH_PAD_DENOISE_CAP_L5

extern QueueHandle_t command_queue; // Cola de colores (task_b)

static bool touch_held[TOUCH_BUTTON_NUM] = {0};
static uint32_t umbral_touch[TOUCH_BUTTON_NUM];

static const touch_pad_t botones_touch[TOUCH_BUTTON_NUM] = {
    TOUCH_PAD_NUM2,   // TP2 - play/pause
    TOUCH_PAD_NUM11,  // TP3 - network
    TOUCH_PAD_NUM5,   // TP4 - record
    TOUCH_PAD_NUM4,   // Guard sensor
    TOUCH_PAD_NUM1,   // TP5 - volume up
    TOUCH_PAD_NUM3    // TP6 - volume down

};


static const led_rgb_evento_t color_touch[TOUCH_BUTTON_NUM] = {
    LED_EVENT_VERDE,
    LED_EVENT_ROJO,
    LED_EVENT_CIAN,
    LED_EVENT_AMARILLO,
    LED_EVENT_BLANCO,
    LED_EVENT_AZUL
};

static const audio_cmd_t comandos_touch[TOUCH_BUTTON_NUM] = {
    CMD_PLAY,
    CMD_STOP,
    CMD_NEXT,
    CMD_PREV,
    CMD_VOL_UP,
    CMD_VOL_DOWN
};

void touch_polling_init(void)
{
    ESP_LOGI(TAG, "Inicializando touch polling");

     // 1. Reiniciar y re-inicializar touch_pad
    touch_pad_deinit(); // Asegura estado limpio
    ESP_ERROR_CHECK(touch_pad_init());

    // 2. Configurar tensión de referencia
    ESP_ERROR_CHECK(touch_pad_set_voltage(
        TOUCH_HVOLT_2V7,
        TOUCH_LVOLT_0V5,
        TOUCH_HVOLT_ATTEN_0V5
    ));

    // 3. Configurar canales en uso
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        ESP_ERROR_CHECK(touch_pad_config(botones_touch[i]));
    }

    // 4. Opcional: ajustar parámetros adicionales
    ESP_ERROR_CHECK(touch_pad_set_charge_discharge_times(TOUCH_PAD_MEASURE_CYCLE_DEFAULT));
    ESP_ERROR_CHECK(touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER));

    // 5. Denoise
    touch_pad_denoise_t denoise_cfg = {
        .grade = TOUCH_DENOISE_GRADE,
        .cap_level = TOUCH_DENOISE_CAP_LEVEL
    };
    ESP_ERROR_CHECK(touch_pad_denoise_set_config(&denoise_cfg));
    ESP_ERROR_CHECK(touch_pad_denoise_enable());

    // 6. Filtro IIR
    touch_filter_config_t filter_cfg = {
        .mode = TOUCH_PAD_FILTER_IIR_8,
        .debounce_cnt = 1,
        .noise_thr = 1,
        .jitter_step = 1,
        .smh_lvl = TOUCH_PAD_SMOOTH_IIR_2
    };
    ESP_ERROR_CHECK(touch_pad_filter_set_config(&filter_cfg));
    ESP_ERROR_CHECK(touch_pad_filter_enable());

    // 7. Iniciar FSM
    ESP_ERROR_CHECK(touch_pad_fsm_start());

    // 8. Esperar estabilización antes de leer benchmarks
    vTaskDelay(pdMS_TO_TICKS(300));

    // 9. Reset benchmark una sola vez por canal
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        ESP_ERROR_CHECK(touch_pad_reset_benchmark(botones_touch[i]));
    }

    // 10. Esperar estabilización del benchmark
    vTaskDelay(pdMS_TO_TICKS(300));

    // 11. Leer benchmarks y establecer umbrales
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        uint32_t benchmark = 0;
        esp_err_t err = touch_pad_read_benchmark(botones_touch[i], &benchmark);
        if (err != ESP_OK || benchmark == 0 || benchmark >= BENCHMARK_INVALID_MAX) {
            ESP_LOGW(TAG, "Canal TCH[%d]: benchmark inválido (%" PRIu32 "), se usará default 10000", i, benchmark);
            benchmark = 10000;
        } else if (benchmark < BENCHMARK_MIN_VALID) {
            ESP_LOGW(TAG, "Canal TCH[%d]: benchmark demasiado bajo (%" PRIu32 "), ajustado a mínimo válido", i, benchmark);
            benchmark = BENCHMARK_MIN_VALID;
        }

        float threshold = benchmark * THRESHOLD_RATIO;
        umbral_touch[i] = (uint32_t)threshold;
        ESP_ERROR_CHECK(touch_pad_set_thresh(botones_touch[i], umbral_touch[i]));

        ESP_LOGI(TAG, "Canal TCH[%d]: benchmark=%" PRIu32 ", umbral=%.0f", i, benchmark, threshold);
    }

    ESP_LOGI(TAG, "Touch polling inicializado correctamente.");
}

bool touch_polling_read(int button_index)
{
    uint32_t val;
    esp_err_t err = touch_pad_read_raw_data(botones_touch[button_index], &val);
    ESP_ERROR_CHECK(err);

    return val > umbral_touch[button_index];;
}
void touch_polling_bucle(void)
{
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        bool pressed = touch_polling_read(i);

        if (pressed && !touch_held[i]) {
            touch_held[i] = true;

            color_event_t color_cmd = {
                .color = color_touch[i],
                .delay_seconds = 0.1
            };
            
            xQueueSend(command_queue, &color_cmd, 0); 

            audio_player_send_cmd(comandos_touch[i]);
            
            ESP_LOGI(TAG, "Botón %d presionado → comando: %s", i, 
            (comandos_touch[i] == CMD_NEXT) ? "CMD_NEXT" :
            (comandos_touch[i] == CMD_PLAY) ? "CMD_PLAY" :
            (comandos_touch[i] == CMD_STOP) ? "CMD_STOP" :
            (comandos_touch[i] == CMD_PREV) ? "CMD_PREV" :
            (comandos_touch[i] == CMD_VOL_UP) ? "CMD_VOL_UP" :
            (comandos_touch[i] == CMD_VOL_DOWN) ? "CMD_VOL_DOWN" :
            "DESCONOCIDO"
);
            esp_rom_delay_us(150000);
        }
        else if (!pressed && touch_held[i]) {
            touch_held[i] = false;
        }
    }
}

/* esta la usamos para ver los valores leidos en tiempo real
void touch_polling_bucle(void)
{
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        uint32_t raw_val;
        esp_err_t err = touch_pad_read_raw_data(botones_touch[i], &raw_val);

        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Error al leer canal TCH[%d]: %s", i, esp_err_to_name(err));
            continue;
        }

        // Log de valores en tiempo real
        ESP_LOGI(TAG, "TCH[%d] → raw=%" PRIu32 " | umbral=%" PRIu32, i, raw_val, umbral_touch[i]);

        bool pressed = (raw_val < umbral_touch[i]);

        if (pressed && !touch_held[i]) {
            touch_held[i] = true;

            ESP_LOGI(TAG, "TCH[%d] PRESIONADO", i);

            color_event_t color_cmd = {
                .color = color_touch[i],
                .delay_seconds = 0.1f
            };
            xQueueSend(command_queue, &color_cmd, 0);

            audio_player_send_cmd(comandos_touch[i]);

            // Pequeño delay para evitar doble lectura
            esp_rom_delay_us(150000);
        }
        else if (!pressed && touch_held[i]) {
            touch_held[i] = false;
            ESP_LOGI(TAG, "TCH[%d] liberado", i);
        }
    }
}
*/
void task_touch(void *pvParameters)
{
    while (1) {
        touch_polling_bucle();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
