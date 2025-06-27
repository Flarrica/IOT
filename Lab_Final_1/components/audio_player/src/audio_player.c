#include "audio_player.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_check.h"

#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "es8311.h"

static const char *TAG = "audio_player";
static i2s_chan_handle_t tx_handle = NULL;

QueueHandle_t audio_event_queue = NULL;

extern const uint8_t music_pcm_start[] asm("_binary_Sonido3_pcm_start");
extern const uint8_t music_pcm_end[]   asm("_binary_Sonido3_pcm_end");

static esp_err_t es8311_codec_init(void)
{
#if !defined(CONFIG_EXAMPLE_BSP)
    const i2c_config_t es_i2c_cfg = {
        .sda_io_num = I2C_SDA_IO,
        .scl_io_num = I2C_SCL_IO,
        .mode = I2C_MODE_MASTER,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    ESP_RETURN_ON_ERROR(i2c_param_config(I2C_NUM, &es_i2c_cfg), TAG, "config i2c failed");
    ESP_RETURN_ON_ERROR(i2c_driver_install(I2C_NUM, I2C_MODE_MASTER, 0, 0, 0), TAG, "install i2c driver failed");
#else
    ESP_ERROR_CHECK(bsp_i2c_init());
#endif

    es8311_handle_t es_handle = es8311_create(I2C_NUM, ES8311_ADDRRES_0);
    ESP_RETURN_ON_FALSE(es_handle, ESP_FAIL, TAG, "es8311 create failed");

    const es8311_clock_config_t es_clk = {
        .mclk_inverted = false,
        .sclk_inverted = false,
        .mclk_from_mclk_pin = true,
        .mclk_frequency = EXAMPLE_MCLK_FREQ_HZ,
        .sample_frequency = EXAMPLE_SAMPLE_RATE
    };

    ESP_ERROR_CHECK(es8311_init(es_handle, &es_clk, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16));
    ESP_RETURN_ON_ERROR(es8311_sample_frequency_config(es_handle, EXAMPLE_SAMPLE_RATE * EXAMPLE_MCLK_MULTIPLE, EXAMPLE_SAMPLE_RATE), TAG, "set es8311 sample frequency failed");
    ESP_RETURN_ON_ERROR(es8311_voice_volume_set(es_handle, EXAMPLE_VOICE_VOLUME, NULL), TAG, "set es8311 volume failed");
    ESP_RETURN_ON_ERROR(es8311_microphone_config(es_handle, false), TAG, "set es8311 microphone failed");

    return ESP_OK;
}

static esp_err_t i2s_driver_init(void)
{
#if !defined(CONFIG_EXAMPLE_BSP)
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM, I2S_ROLE_MASTER);
    chan_cfg.auto_clear = true;
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(EXAMPLE_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_MCK_IO,
            .bclk = I2S_BCK_IO,
            .ws = I2S_WS_IO,
            .dout = I2S_DO_IO,
            .din = I2S_DI_IO,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    std_cfg.clk_cfg.mclk_multiple = EXAMPLE_MCLK_MULTIPLE;

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));
#else
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(EXAMPLE_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = BSP_I2S_GPIO_CFG,
    };
    std_cfg.clk_cfg.mclk_multiple = EXAMPLE_MCLK_MULTIPLE;
    ESP_ERROR_CHECK(bsp_audio_init(&std_cfg, &tx_handle, NULL));
    ESP_ERROR_CHECK(bsp_audio_poweramp_enable(true));
#endif
    return ESP_OK;
}

static void task_audio_player(void *args)
{
    audio_cmd_t cmd;
    static int current_track = 0;
    static int volume = EXAMPLE_VOICE_VOLUME;
    size_t bytes_written;
    uint8_t *ptr;

    while (1) {
        if (xQueueReceive(audio_event_queue, &cmd, portMAX_DELAY)) {
            switch (cmd) {
                case CMD_PLAY:
                    ESP_LOGI(TAG, "PLAY received (track %d)", current_track);
                    ptr = (uint8_t *)music_pcm_start;
                    i2s_channel_enable(tx_handle);
                    while (ptr < music_pcm_end) {
                        i2s_channel_write(tx_handle, ptr, music_pcm_end - ptr, &bytes_written, portMAX_DELAY);
                        ptr += bytes_written;
                    }
                    break;

                case CMD_STOP:
                    ESP_LOGI(TAG, "STOP received");
                    i2s_channel_disable(tx_handle);
                    break;

                case CMD_NEXT:
                    current_track = (current_track + 1) % 9;  // suponiendo 9 pistas
                    ESP_LOGI(TAG, "NEXT: track %d", current_track);
                    break;

                case CMD_PREV:
                    current_track = (current_track - 1 + 9) % 9;
                    ESP_LOGI(TAG, "PREV: track %d", current_track);
                    break;

                case CMD_VOL_UP:
                    volume = (volume < 100) ? volume + 10 : 100;
                    ESP_LOGI(TAG, "VOL UP: %d", volume);
                    break;

                case CMD_VOL_DOWN:
                    volume = (volume > 0) ? volume - 10 : 0;
                    ESP_LOGI(TAG, "VOL DOWN: %d", volume);
                    break;

                default:
                    ESP_LOGW(TAG, "Comando desconocido: %d", cmd);
                    break;
            }
        }
    }
}

esp_err_t audio_player_init(void)
{
    ESP_LOGI(TAG, "Inicializando driver I2S...");
    ESP_RETURN_ON_ERROR(i2s_driver_init(), TAG, "Fallo init I2S");

    ESP_LOGI(TAG, "Inicializando codec ES8311...");
    ESP_RETURN_ON_ERROR(es8311_codec_init(), TAG, "Fallo init codec");

#if EXAMPLE_PA_CTRL_IO >= 0
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = (1ULL << EXAMPLE_PA_CTRL_IO),
        .mode = GPIO_MODE_OUTPUT,
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_cfg));
    ESP_ERROR_CHECK(gpio_set_level(EXAMPLE_PA_CTRL_IO, 1));
#endif

    audio_event_queue = xQueueCreate(8, sizeof(audio_event_t));
    xTaskCreate(task_audio_player, "task_audio_player", 4096, NULL, 5, NULL);

    return ESP_OK;
}
