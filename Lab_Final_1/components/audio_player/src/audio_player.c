// ------------------------
// INCLUDES
// ------------------------

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <dirent.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "audio_player.h"

#include "esp_log.h"
#include "esp_check.h"

#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "es8311.h"

#include "esp_spiffs.h"

// ------------------------
// DEFINES Y GLOBALES
// ------------------------

#define MAX_PLAYLIST 10
#define MAX_FILENAME_LEN 255
#define MAX_PATH_LEN (8 + MAX_FILENAME_LEN + 1)

static const char *TAG = "audio_player";
static i2s_chan_handle_t tx_handle = NULL;
static es8311_handle_t es_handle = NULL;

QueueHandle_t audio_event_queue = NULL;

static char playlist[MAX_PLAYLIST][MAX_PATH_LEN];
static int playlist_size = 0;

// ------------------------
// MONTAJE DE SPIFFS
// ------------------------

esp_err_t spiffs_init(void) {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "spiffs",
        .max_files = 10,
        .format_if_mount_failed = false
    };

    ESP_RETURN_ON_ERROR(esp_vfs_spiffs_register(&conf), TAG, "Fallo al montar SPIFFS");

    size_t total = 0, used = 0;
    esp_err_t info_ret = esp_spiffs_info("spiffs", &total, &used);
    if (info_ret == ESP_OK) {
        ESP_LOGI(TAG, "SPIFFS montado: %d KB totales, %d KB usados", total / 1024, used / 1024);
    } else {
        ESP_LOGW(TAG, "No se pudo obtener información de SPIFFS (err=0x%x)", info_ret);
    }

    return ESP_OK;
}

// ------------------------
// INICIALIZACIÓN DEL CÓDEC ES8311
// ------------------------

static esp_err_t es8311_codec_init(void) {
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

    es_handle = es8311_create(I2C_NUM, ES8311_ADDRRES_0);
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
    ESP_LOGI(TAG, "Volumen inicial seteado: %d", EXAMPLE_VOICE_VOLUME);
    int vol;
    if (es8311_voice_volume_get(es_handle, &vol) == ESP_OK) {
        ESP_LOGI(TAG, "Volumen actual en códec: %d", vol);
    } else {
        ESP_LOGW(TAG, "No se pudo obtener el volumen desde el códec");
    }
    ESP_RETURN_ON_ERROR(es8311_microphone_config(es_handle, false), TAG, "set es8311 microphone failed");

    return ESP_OK;
}

// ------------------------
// INICIALIZACIÓN DE DRIVER I2S
// ------------------------

static esp_err_t i2s_driver_init(void) {
#if !defined(CONFIG_EXAMPLE_BSP)
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM, I2S_ROLE_MASTER);
    chan_cfg.auto_clear = true;
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(EXAMPLE_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
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

static audio_state_t player_state = PLAYER_STOPPED;

audio_state_t audio_player_get_state(void) {
    return player_state;
}

void audio_player_send_cmd(audio_cmd_t cmd) {
    if (audio_event_queue != NULL) {
        xQueueSend(audio_event_queue, &cmd, 0);
    } else {
        ESP_LOGW(TAG, "Cola de comandos no inicializada");
    }
}

static void task_audio_player(void *args) {
    audio_cmd_t cmd;
    static int current_track = 0;
    static int volume = EXAMPLE_VOICE_VOLUME;
    size_t bytes_written;

    while (1) {
        if (xQueueReceive(audio_event_queue, &cmd, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Comando recibido: %d", cmd);
            switch (cmd) {
                case CMD_PLAY:
                    ESP_LOGI(TAG, "PLAY received (track %d)", current_track);
                    FILE *f = fopen(playlist[current_track], "r");
                    if (!f) {
                        ESP_LOGE(TAG, "No se pudo abrir el archivo: %s", playlist[current_track]);
                        break;
                    }
                    fseek(f, 44, SEEK_SET);
                    uint8_t buffer[512];
                    size_t read_bytes;

                    if (player_state != PLAYER_PLAYING) {
                        ESP_LOGW(TAG, "PLAY: Habilitando canal I2S.");
                        ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));
                    }

                    player_state = PLAYER_PLAYING;
                    
                    while ((read_bytes = fread(buffer, 1, sizeof(buffer), f)) > 0) {
                        //ESP_LOGI(TAG, "Leídos %d bytes del archivo", read_bytes);
                        esp_err_t err = i2s_channel_write(tx_handle, buffer, read_bytes, &bytes_written, portMAX_DELAY);
                        if (err != ESP_OK) {
                            ESP_LOGE(TAG, "Error al escribir en I2S: 0x%x", err);
                        } else {
                            //ESP_LOGI(TAG, "Escritos %d bytes al canal I2S", bytes_written);
                        }
                    }
                    
                    fclose(f);
                    ESP_LOGI(TAG, "Reproducción finalizada");

                    current_track = (current_track + 1) % playlist_size;
                    audio_player_send_cmd(CMD_PLAY);
                    break;

                case CMD_STOP:
                    if (player_state != PLAYER_PLAYING) {
                        ESP_LOGW(TAG, "Reproductor no está en reproducción. STOP ignorado.");
                        break;
                    }
                    ESP_LOGI(TAG, "STOP received");
                    i2s_channel_disable(tx_handle);
                    player_state = PLAYER_STOPPED;
                    break;

                case CMD_NEXT:
                    current_track = (current_track + 1) % playlist_size;
                    ESP_LOGI(TAG, "NEXT: track %d", current_track);
                    break;

                case CMD_PREV:
                    current_track = (current_track - 1 + playlist_size) % playlist_size;
                    ESP_LOGI(TAG, "PREV: track %d", current_track);
                    break;

                case CMD_VOL_UP:
                    if (volume < 100) volume += 10;
                    if (es_handle) {
                        esp_err_t err = es8311_voice_volume_set(es_handle, volume, NULL);
                        if (err == ESP_OK) {
                            ESP_LOGI(TAG, "VOL UP: volumen seteado a %d", volume);
                            int vol_check = -1;
                            if (es8311_voice_volume_get(es_handle, &vol_check) == ESP_OK) {
                                ESP_LOGI(TAG, "VOL UP: volumen confirmado en codec: %d", vol_check);
                            }
                        }
                    }
                    break;

                case CMD_VOL_DOWN:
                    if (volume > 0) volume -= 10;
                    if (es_handle) {
                        esp_err_t err = es8311_voice_volume_set(es_handle, volume, NULL);
                        if (err == ESP_OK) {
                            ESP_LOGI(TAG, "VOL DOWN: volumen seteado a %d", volume);
                            int vol_check = -1;
                            if (es8311_voice_volume_get(es_handle, &vol_check) == ESP_OK) {
                                ESP_LOGI(TAG, "VOL DOWN: volumen confirmado en codec: %d", vol_check);
                            }
                        }
                    }
                    break;
            }
        }
    }
}

static void load_playlist_from_spiffs(void) {
    DIR *dir = opendir("/spiffs");
    struct dirent *entry;

    if (!dir) {
        ESP_LOGE(TAG, "No se pudo abrir /spiffs");
        return;
    }

    while ((entry = readdir(dir)) != NULL && playlist_size < MAX_PLAYLIST) {
        if (strstr(entry->d_name, ".wav")) {
            snprintf(playlist[playlist_size], sizeof(playlist[0]), "/spiffs/%s", entry->d_name);
            ESP_LOGI(TAG, "Archivo agregado a playlist: %s", playlist[playlist_size]);
            playlist_size++;
        }
    }

    ESP_LOGI(TAG, "Archivos cargados en la playlist (tamaño: %d):", playlist_size);
    for (int i = 0; i < playlist_size; i++) {
        ESP_LOGI(TAG, "  [%d] %s", i, playlist[i]);
    }
    closedir(dir);
}

esp_err_t audio_player_init(void) {
    ESP_RETURN_ON_ERROR(spiffs_init(), TAG, "Fallo al montar SPIFFS");

    load_playlist_from_spiffs();

    ESP_LOGI(TAG, "Inicializando driver I2S...");
    ESP_RETURN_ON_ERROR(i2s_driver_init(), TAG, "Fallo init I2S");

    ESP_LOGI(TAG, "Inicializando codec ES8311...");
    ESP_RETURN_ON_ERROR(es8311_codec_init(), TAG, "Fallo init codec");

    audio_event_queue = xQueueCreate(8, sizeof(audio_cmd_t));
    xTaskCreate(task_audio_player, "task_audio_player", 4096, NULL, configMAX_PRIORITIES - 1, NULL);

    int vol_check;
    if (es8311_voice_volume_get(es_handle, &vol_check) == ESP_OK) {
        ESP_LOGI(TAG, "Volumen verificado al salir de codec_init: %d", vol_check);
    }

    // ------------------------
    // ACTIVAR AMPLIFICADOR
    // ------------------------
    gpio_reset_pin(EXAMPLE_PA_CTRL_IO);
    gpio_set_direction(EXAMPLE_PA_CTRL_IO, GPIO_MODE_OUTPUT);
    gpio_set_level(EXAMPLE_PA_CTRL_IO, 1);  // Habilita el PA
    ESP_LOGI(TAG, "Amplificador de potencia habilitado (GPIO10 en HIGH)");

    return ESP_OK;
}