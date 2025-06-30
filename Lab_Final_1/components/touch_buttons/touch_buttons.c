#include "touch_buttons.h"

#include "driver/touch_pad.h"
#include "esp_log.h"
#include "esp_rom_sys.h"        // Para esp_rom_delay_us()
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led_rgb.h"
#include "audio_player.h"

#define TAG "TOUCH_BUTTONS"
#define TOUCH_BUTTON_NUM 6
#define TOUCH_THRESHOLD 60000

static const touch_pad_t botones_touch[TOUCH_BUTTON_NUM] = {
    TOUCH_PAD_NUM1,  // VOL_UP
    TOUCH_PAD_NUM6,  // PHOTO
    TOUCH_PAD_NUM5,  // NETWORK
    TOUCH_PAD_NUM4,  // RECORD
    TOUCH_PAD_NUM3,  // PLAY/PAUSE
    TOUCH_PAD_NUM2   // VOL_DOWN
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
    CMD_VOL_UP,     // VOL_UP
    CMD_NEXT,       // PHOTO
    CMD_PREV,       // NETWORK
    CMD_STOP,       // RECORD
    CMD_PLAY,       // PLAY/PAUSE
    CMD_VOL_DOWN    // VOL_DOWN
};

void touch_polling_init(void)
{
    ESP_LOGI(TAG, "Inicializando touch polling...");

    ESP_ERROR_CHECK(touch_pad_init());

    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        ESP_ERROR_CHECK(touch_pad_config(botones_touch[i]));
    }

    ESP_ERROR_CHECK(touch_pad_set_charge_discharge_times(10));

    touch_filter_config_t filter_cfg = {
        .mode = TOUCH_PAD_FILTER_IIR_8,
        .debounce_cnt = 1,
        .noise_thr = 0,
        .jitter_step = 0,
        .smh_lvl = TOUCH_PAD_SMOOTH_IIR_2,
    };

    ESP_ERROR_CHECK(touch_pad_filter_set_config(&filter_cfg));
    ESP_ERROR_CHECK(touch_pad_filter_enable());

    ESP_ERROR_CHECK(touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER));
    ESP_ERROR_CHECK(touch_pad_fsm_start());

    ESP_LOGI(TAG, "Touch polling inicializado correctamente.");
}

bool touch_polling_read(int button_index)
{
    uint32_t val;
    ESP_ERROR_CHECK(touch_pad_read_raw_data(botones_touch[button_index], &val));
    return val < TOUCH_THRESHOLD;
}

void touch_polling_bucle(void)
{
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        if (touch_polling_read(i)) {
            led_rgb_set_event(color_touch[i]);
            audio_player_send_cmd(comandos_touch[i]);
            esp_rom_delay_us(150000); // 150ms de antirebote suave y tiempo de respuesta amigable
        }
    }
}

void task_touch(void *pvParameters)
{
    while (1) {
        touch_polling_bucle();
        vTaskDelay(pdMS_TO_TICKS(30)); // retardo básico de sondeo
    }
}