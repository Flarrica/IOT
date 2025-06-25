#include <stdio.h>
#include <string.h>

#include "audio_player.h"
#include "esp_log.h"
#include "driver/i2s_std.h"
#include "es8311.h"
#include "esp_check.h"

#include "freertos/queue.h"
#include "freertos/semphr.h"

static const char *TAG = "audio_player";

#define MAX_PLAYLIST 9
static const char *playlist[MAX_PLAYLIST] = {
    "/spiffs/ch_1.wav",
    "/spiffs/cv_1.wav",
    "/spiffs/cv_2.wav",
    "/spiffs/dk_1.wav",
    "/spiffs/dk_2.wav",
    "/spiffs/dk_3.wav",
    "/spiffs/gk_1.wav",
    "/spiffs/rei_1.wav",
    "/spiffs/zd_1.wav",
};

static int current_track = 0;
static bool is_playing = false;
static int volume = 80;

static QueueHandle_t audio_event_queue = NULL;

static i2s_chan_handle_t tx_chan = NULL;
static es8311_handle_t es_handle = NULL;

#define SAMPLE_RATE     16000
#define BITS_PER_SAMPLE 8
#define MCLK_MULTIPLE   I2S_MCLK_MULTIPLE_256
#define BUFFER_SIZE     512

// Pines I2S e I2C (ajustado a LyraT-8311A v1.2)
#define I2S_NUM         0
#define I2S_BCK_IO      39
#define I2S_WS_IO       21
#define I2S_DO_IO       3
#define I2S_DI_IO       1
#define I2S_MCK_IO      41
#define I2C_SCL_IO      7
#define I2C_SDA_IO      8
#define I2C_NUM         0

esp_err_t audio_player_init(void)
{
    // I2S
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM, I2S_ROLE_MASTER);
    chan_cfg.auto_clear = true;
    ESP_RETURN_ON_ERROR(i2s_new_channel(&chan_cfg, &tx_chan, NULL), TAG, "i2s_new_channel failed");

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_SLOT_BIT_WIDTH_8BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_MCK_IO,
            .bclk = I2S_BCK_IO,
            .ws = I2S_WS_IO,
            .dout = I2S_DO_IO,
            .din = I2S_DI_IO,
            .invert_flags = {0}
        }
    };
    std_cfg.clk_cfg.mclk_multiple = MCLK_MULTIPLE;
    ESP_RETURN_ON_ERROR(i2s_channel_init_std_mode(tx_chan, &std_cfg), TAG, "i2s_channel_init_std_mode failed");
    ESP_RETURN_ON_ERROR(i2s_channel_enable(tx_chan), TAG, "i2s_channel_enable failed");

    // I2C
    i2c_config_t i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_IO,
        .scl_io_num = I2C_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000
    };
    ESP_RETURN_ON_ERROR(i2c_param_config(I2C_NUM, &i2c_cfg), TAG, "i2c_param_config failed");
    ESP_RETURN_ON_ERROR(i2c_driver_install(I2C_NUM, i2c_cfg.mode, 0, 0, 0), TAG, "i2c_driver_install failed");

    // ES8311
    es_handle = es8311_create(I2C_NUM, ES8311_ADDRRES_0);
    ESP_RETURN_ON_FALSE(es_handle, ESP_FAIL, TAG, "es8311_create failed");

    es8311_clock_config_t clk_cfg = {
        .mclk_inverted = false,
        .sclk_inverted = false,
        .mclk_from_mclk_pin = true,
        .mclk_frequency = SAMPLE_RATE * MCLK_MULTIPLE,
        .sample_frequency = SAMPLE_RATE,
    };
    ESP_RETURN_ON_ERROR(es8311_init(es_handle, &clk_cfg, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16), TAG, "es8311_init failed");
    ESP_RETURN_ON_ERROR(es8311_voice_volume_set(es_handle, 80, NULL), TAG, "set volume failed");
    ESP_RETURN_ON_ERROR(es8311_microphone_config(es_handle, false), TAG, "disable mic failed");

    ESP_LOGI(TAG, "audio player initialized");
    return ESP_OK;
}

esp_err_t audio_player_play_file(const char *filepath)
{
    FILE *f = fopen(filepath, "rb");
    if (!f) {
        ESP_LOGE(TAG, "No se pudo abrir el archivo: %s", filepath);
        return ESP_FAIL;
    }

    fseek(f, 44, SEEK_SET); // omitir encabezado WAV
    uint8_t buffer[BUFFER_SIZE];
    size_t r, w;

    while ((r = fread(buffer, 1, BUFFER_SIZE, f)) > 0) {
        if (!is_playing) break; // Con esto da el STOP!!!!
        i2s_channel_write(tx_chan, buffer, r, &w, portMAX_DELAY);
    }

    fclose(f);
    ESP_LOGI(TAG, "Reproducción finalizada: %s", filepath);
    return ESP_OK;
}

void task_audio_player(void *pvParams)
{
    audio_event_t evt;
    audio_event_queue = xQueueCreate(5, sizeof(audio_event_t));
    if (!audio_event_queue) {
        ESP_LOGE(TAG, "No se pudo crear la cola de eventos de audio");
        vTaskDelete(NULL);
    }

    audio_player_init();

    while (1) {
        if (xQueueReceive(audio_event_queue, &evt, portMAX_DELAY)) {
            switch (evt.cmd) {
                case AUDIO_CMD_PLAY:
                    if (!is_playing) {
                        is_playing = true;
                        audio_player_play_file(playlist[current_track]);
                        is_playing = false;
                    }
                    break;

                case AUDIO_CMD_STOP:
                    is_playing = false;  // Esto fuerza que el bucle de reproducción se interrumpa
                    break;

                case AUDIO_CMD_NEXT:
                    current_track = (current_track + 1) % MAX_PLAYLIST;
                    is_playing = true;
                    audio_player_play_file(playlist[current_track]);
                    is_playing = false;
                    break;

                case AUDIO_CMD_PREV:
                    current_track = (current_track - 1 + MAX_PLAYLIST) % MAX_PLAYLIST;
                    is_playing = true;
                    audio_player_play_file(playlist[current_track]);
                    is_playing = false;
                    break;

                case AUDIO_CMD_VOL_UP:
                    if (volume < 100) volume += 10;
                    es8311_voice_volume_set(es_handle, volume, NULL);
                    break;

                case AUDIO_CMD_VOL_DOWN:
                    if (volume > 0) volume -= 10;
                    es8311_voice_volume_set(es_handle, volume, NULL);
                    break;
            }
        }
    }
}