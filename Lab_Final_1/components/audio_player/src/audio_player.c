// ------------------------
// INCLUDES
// ------------------------

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "audio_player.h"
#include "led_rgb.h"
#include "shared_lib.h"

#include "esp_log.h"
#include "esp_check.h"

#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "es8311.h"
#include "logger.h"

#include "esp_spiffs.h"
#include "task_mqtt.h"

// ------------------------
// DEFINES Y GLOBALES
// ------------------------
#define PASO_PERCEPTUAL 2 //Pasos para cambio de volumen.  

static const char *TAG = "audio_player";

color_event_t event_play  = { .color = LED_EVENT_VERDE,    .delay_seconds = 0 };
color_event_t event_play_1  = { .color = LED_EVENT_VERDE,    .delay_seconds = 1 };
color_event_t event_pause = { .color = LED_EVENT_AZUL,     .delay_seconds = 0 };
color_event_t event_stop  = { .color = LED_EVENT_ROJO,     .delay_seconds = 0 };
color_event_t event_PREV  = { .color = LED_EVENT_BLANCO,     .delay_seconds = 0 };
color_event_t event_NEXT  = { .color = LED_EVENT_AMARILLO,     .delay_seconds = 0 };


static i2s_chan_handle_t tx_handle = NULL;
static es8311_handle_t es_handle = NULL;

QueueHandle_t audio_event_queue = NULL;

static char playlist[MAX_PLAYLIST][MAX_PATH_LEN];
static int playlist_size = 0;

static SemaphoreHandle_t current_track_mutex;
SemaphoreHandle_t volume_mutex; //Porque dos task comparten el recurso
extern SemaphoreHandle_t i2c_mutex;
extern QueueHandle_t color_queue;
// ------------------------
// VARIABLES VOLATILES
// ------------------------
volatile audio_state_t player_state = PLAYER_STOPPED;
volatile int volumen = EXAMPLE_VOICE_VOLUME;
volatile int perceptual_vol = 0;
volatile int next_track_index = -1;

#define VOLUMEN_MAXIMO_USUARIO 100

// ------------------------
// VARIABLES
// ------------------------
static int current_track = 0;
static const char* command_to_str(audio_cmd_t cmd) {
    switch (cmd) {
        case CMD_PLAY:     return "CMD_PLAY";
        case CMD_STOP:     return "CMD_STOP";
        case CMD_PAUSE:    return "CMD_PAUSE";
        case CMD_NEXT:     return "CMD_NEXT";
        case CMD_PREV:     return "CMD_PREV";
        case CMD_VOL_UP:   return "CMD_VOL_UP";
        case CMD_VOL_DOWN: return "CMD_VOL_DOWN";
        default:           return "CMD_UNKNOWN";
    }
}

// ------------------------
// FUNCIONES AUXILIARES - TRACKS
// ------------------------

void track_prev(void) {
    if (xSemaphoreTake(current_track_mutex, portMAX_DELAY)) {
        current_track = (current_track - 1 + playlist_size) % playlist_size;
        xSemaphoreGive(current_track_mutex);
    }
}

void track_next(void) {
    if (xSemaphoreTake(current_track_mutex, portMAX_DELAY)) {
        current_track = (current_track + 1) % playlist_size;
        xSemaphoreGive(current_track_mutex);
    }
}

void track_set(int idx) {
    if (xSemaphoreTake(current_track_mutex, portMAX_DELAY)) {
        current_track = idx;
        xSemaphoreGive(current_track_mutex);
    }
}

int track_get(void) {
    int track = 0;
    if (xSemaphoreTake(current_track_mutex, portMAX_DELAY)) {
        track = current_track;
        xSemaphoreGive(current_track_mutex);
    }
    return track;
}



// ------------------------
// MAPEO LOGARITMICO DE VOLUMEN
// ------------------------
static int map_volume_perceptual(int vol_user) {
    if (vol_user > VOLUMEN_MAXIMO_USUARIO) vol_user = VOLUMEN_MAXIMO_USUARIO;

    float norm = vol_user / (float)VOLUMEN_MAXIMO_USUARIO;
    float scaled = powf(norm, 2.2f);  // curva perceptual
    return (int)roundf(scaled * EXAMPLE_VOICE_VOLUME);
}

static int map_volume_user_from_perceptual(int perceptual) {
    if (perceptual > EXAMPLE_VOICE_VOLUME) perceptual = EXAMPLE_VOICE_VOLUME;

    float norm = perceptual / (float)EXAMPLE_VOICE_VOLUME;
    float inv_scaled = powf(norm, 1.0f / 2.2f);  // inversa de pow(., 2.2)
    return (int)roundf(inv_scaled * VOLUMEN_MAXIMO_USUARIO);
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

    // Toma el mutex antes de acceder al codec
    if (xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(100))) {

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
        volumen = (EXAMPLE_VOICE_VOLUME > VOLUMEN_MAXIMO_USUARIO) ? VOLUMEN_MAXIMO_USUARIO : EXAMPLE_VOICE_VOLUME;
        perceptual_vol = map_volume_perceptual(volumen);
        ESP_RETURN_ON_ERROR(es8311_voice_volume_set(es_handle, perceptual_vol, NULL), TAG, "set es8311 volume failed");

        ESP_LOGI(TAG, "Volumen inicial seteado: %d", EXAMPLE_VOICE_VOLUME);
        int vol;
        if (es8311_voice_volume_get(es_handle, &vol) == ESP_OK) {
            ESP_LOGI(TAG, "Volumen actual en códec: %d", vol);
        } else {
            ESP_LOGW(TAG, "No se pudo obtener el volumen desde el códec");
        }

        ESP_RETURN_ON_ERROR(es8311_microphone_config(es_handle, false), TAG, "set es8311 microphone failed");

        // Libera el mutex al terminar
        xSemaphoreGive(i2c_mutex);

    } else {
        ESP_LOGE(TAG, "No se pudo tomar el mutex para inicializar el códec");
        return ESP_ERR_TIMEOUT;
    }

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

// ------------------------
// DEVUELVE ESTADO DEL REPRODUCTOR
// ------------------------

audio_state_t audio_player_get_state(void) {
    return player_state;
}

// ------------------------
// LEEE HEADER DE WAV
// ------------------------
bool wav_read_header(FILE *f, wav_header_t *header) {
    size_t read = fread(header, sizeof(wav_header_t), 1, f);
    if (read != 1) return false;

    if (memcmp(header->riff_id, "RIFF", 4) != 0 ||
        memcmp(header->wave_id, "WAVE", 4) != 0 ||
        memcmp(header->fmt_id, "fmt ", 4) != 0 ||
        memcmp(header->data_id, "data", 4) != 0) {
        return false;
    }

    return true;
}

// ------------------------
// ENVIAR COMANDO A COLA DE AUDIO_PLAYER
// ------------------------
void audio_player_send_cmd(audio_cmd_t cmd) {
    if (audio_event_queue != NULL) {
        xQueueSend(audio_event_queue, &cmd, 0);
        switch (cmd) {
        case CMD_PLAY:
            logger_add_event(PLAY);
            break;
        case CMD_PAUSE:
            logger_add_event(PAUSE);
            break;
        case CMD_STOP:
            logger_add_event(STOP);
            break;
        case CMD_NEXT:
            logger_add_event(NEXT);
            break;
        case CMD_PREV:
            logger_add_event(PREVIOUS);
            break;
        default:
            break;
    }

    } else {
        ESP_LOGW(TAG, "Cola de comandos no inicializada");
        
    }
}

// ------------------------
// TAREA AUDIO_COMMANDS
// ------------------------
static void task_audio_commands(void *args){
    audio_cmd_t cmd;
    while (1) {
        if (xQueueReceive(audio_event_queue, &cmd, portMAX_DELAY)) {
            ESP_LOGI("Audio_CMD_task:", "Comando recibido: %s", command_to_str(cmd));
            switch (cmd) {
                case CMD_PLAY: // Llega un comando de PLAY
                    if (player_state == PLAYER_PAUSED || player_state == PLAYER_STOPPED ) {
                        player_state = PLAYER_PLAYING;  // Reanuda. La task del audioplayer se encarga de reanudar lectura escritura
                        ESP_LOGI("Audio_CMD_task:", "Reanudar reproducción.");
                        xQueueSend(color_queue, &event_play, portMAX_DELAY); 
                        }else 
                        if (player_state == PLAYER_PLAYING){
                            player_state = PLAYER_PAUSED;
                            xQueueSend(color_queue, &event_pause, portMAX_DELAY); 
                            ESP_LOGI("Audio_CMD_task: ","Pausar reproducción.");
                        }
                    publicar_estado_reproductor(mqtt_get_client(), estado_reproductor);
                    break;
                case CMD_PAUSE:
                    if (player_state == PLAYER_PLAYING) {
                        player_state = PLAYER_PAUSED;
                        xQueueSend(color_queue, &event_pause, portMAX_DELAY); 
                        ESP_LOGI("Audio_CMD_task: ","Pausar reproducción.");
                    }
                    publicar_estado_reproductor(mqtt_get_client(), estado_reproductor);
                    break;

                case CMD_STOP:
                    player_state = PLAYER_STOPPED;
                    xQueueSend(color_queue, &event_stop, portMAX_DELAY); 
                    publicar_estado_reproductor(mqtt_get_client(), estado_reproductor);
                    break;

                case CMD_NEXT:
                    if (playlist_size > 0) {
                        next_track_index = (track_get() + 1) % playlist_size;
                        player_state = PLAYER_PLAYING;
                        xQueueSend(color_queue, &event_NEXT, portMAX_DELAY);
                        xQueueSend(color_queue, &event_play_1, portMAX_DELAY);
                        ESP_LOGI("Audio_CMD_task:", "CMD_NEXT → nuevo track: %d", next_track_index);
                    }
                    publicar_estado_reproductor(mqtt_get_client(), estado_reproductor);
                    break;
                case CMD_PREV:
                    if (playlist_size > 0) {
                        next_track_index = (track_get() - 1 + playlist_size) % playlist_size;
                        player_state = PLAYER_PLAYING;
                        xQueueSend(color_queue, &event_PREV, portMAX_DELAY);
                        xQueueSend(color_queue, &event_play_1, portMAX_DELAY);
                        ESP_LOGI("Audio_CMD_task:", "CMD_PREV → nuevo track: %d", next_track_index);
                    }
                    publicar_estado_reproductor(mqtt_get_client(), estado_reproductor);
                    break;
                case CMD_VOL_UP:
                    perceptual_vol += PASO_PERCEPTUAL;
                    if (perceptual_vol > EXAMPLE_VOICE_VOLUME) {
                        perceptual_vol = EXAMPLE_VOICE_VOLUME;
                    }
                    volumen = map_volume_user_from_perceptual(perceptual_vol);
                    if (es8311_voice_volume_set(es_handle, perceptual_vol, NULL) == ESP_OK) {
                        ESP_LOGI(TAG, "VOL_UP → volumen perceptual: %d, volumen usuario: %d", perceptual_vol, volumen);
                    } else {
                        ESP_LOGE(TAG, "Error al setear volumen en CMD_VOL_UP");
                    }
                    publicar_estado_reproductor(mqtt_get_client(), estado_reproductor);
                    break;

                case CMD_VOL_DOWN:
                    if (perceptual_vol >= PASO_PERCEPTUAL) {
                        perceptual_vol -= PASO_PERCEPTUAL;
                    } else {
                        perceptual_vol = 0;
                    }
                    volumen = map_volume_user_from_perceptual(perceptual_vol);
                    if (es8311_voice_volume_set(es_handle, perceptual_vol, NULL) == ESP_OK) {
                        ESP_LOGI(TAG, "VOL_UP → volumen perceptual: %d, volumen usuario: %d", perceptual_vol, volumen);
                    } else {
                        ESP_LOGE(TAG, "Error al setear volumen en CMD_VOL_UP");
                    }
                    publicar_estado_reproductor(mqtt_get_client(), estado_reproductor);
                    break;
                default:
                    ESP_LOGW("CMD_TASK", "Comando desconocido: %d", cmd);
                    break;
            }
        }
    }
}

// ------------------------
// TAREA AUDIO_PLAYER
// ------------------------
static void task_audio_player(void *args) {
    size_t bytes_written;
    uint8_t buffer[512];
    size_t read_bytes;
    FILE *f = NULL;

    wav_header_t header;
    uint32_t remaining_bytes = 0;

    while (1) {
        switch (player_state) {
                case PLAYER_PLAYING:
                    if (next_track_index >= 0) {
                        // Cambiar de pista
                        if (f) {
                            fclose(f);
                            f = NULL;
                        }
                        track_set(next_track_index);
                        next_track_index = -1;
                        ESP_LOGI(TAG, "Cambio inmediato a track %d", current_track);
                    }
                    if (!f) {
                        f = fopen(playlist[track_get()], "r");
                        if (!f) {
                            ESP_LOGE("Audio PLAYER", "No se pudo abrir el archivo: %s", playlist[track_get()]);
                            player_state = PLAYER_STOPPED;
                            break;
                        }
                        if (!wav_read_header(f, &header)) {
                            ESP_LOGE("Audio PLAYER", "Archivo WAV inválido: %s", playlist[current_track]);
                            fclose(f);
                            f = NULL;
                            player_state = PLAYER_STOPPED;
                            break;
                        }
                        remaining_bytes = header.data_size;
                        ESP_LOGI("Audio PLAYER", "Reproduciendo track %d", current_track);
                        //SUBO VOLUMEN AL ULTIMO VALOR
                        
                        
                        if (xSemaphoreTake(volume_mutex, portMAX_DELAY)) {
                            perceptual_vol = map_volume_perceptual(volumen);
                            es8311_voice_volume_set(es_handle, perceptual_vol, NULL);
                            xSemaphoreGive(volume_mutex);
                        }

                    }
                    memset(buffer, 0, sizeof(buffer));  // Limpia el buffer antes de cada lectura
                    if (remaining_bytes > 0) {
                        size_t to_read = remaining_bytes > sizeof(buffer) ? sizeof(buffer) : remaining_bytes;
                        read_bytes = fread(buffer, 1, to_read, f);
                        if (read_bytes > 0) {
                            remaining_bytes -= read_bytes;
                            esp_err_t err = i2s_channel_write(tx_handle, buffer, read_bytes, &bytes_written, portMAX_DELAY);
                            if (err != ESP_OK) {
                                ESP_LOGE("Audio PLAYER", "Error al escribir en I2S: 0x%x", err);
                            }
                        }
                    } else {
                        // Reproduce 100 ms de silencio para evitar ruidos
                        memset(buffer, 0, sizeof(buffer));
                        int silencio_ms = 100;
                        int bytes_por_muestra = header.bits_per_sample / 8;
                        int total_muestras = (EXAMPLE_SAMPLE_RATE * silencio_ms) / 1000;
                        int total_bytes = total_muestras * bytes_por_muestra;
                        while (total_bytes > 0) {
                            size_t chunk = total_bytes > sizeof(buffer) ? sizeof(buffer) : total_bytes;
                            i2s_channel_write(tx_handle, buffer, chunk, &bytes_written, portMAX_DELAY);
                            total_bytes -= chunk;
                        }
                        fclose(f);
                        f = NULL;
                        track_next();
                        player_state = PLAYER_PLAYING; // sigue en modo reproducción
                    }
                    break;
                case PLAYER_PAUSED:
                    vTaskDelay(pdMS_TO_TICKS(100));
                    break;

                case PLAYER_STOPPED:
                    if (f) {
                        fclose(f);
                        f = NULL;
                    }
                    vTaskDelay(pdMS_TO_TICKS(100));
                    break;
        }

    }

}

// ------------------------
// CARGAR PLAYLIST DESDE MEMORIA
// ------------------------
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

// ------------------------
// INICIALIZACIÓN DE AUIDO PLAYER
// ------------------------
esp_err_t audio_player_init(void) {
    ESP_RETURN_ON_ERROR(spiffs_init(), TAG, "Fallo al montar SPIFFS");

    load_playlist_from_spiffs();

    ESP_LOGI(TAG, "Inicializando driver I2S...");
    ESP_RETURN_ON_ERROR(i2s_driver_init(), TAG, "Fallo init I2S");

    ESP_LOGI(TAG, "Inicializando codec ES8311...");
    ESP_RETURN_ON_ERROR(es8311_codec_init(), TAG, "Fallo init codec");
    
    ESP_LOGI(TAG, "Habilitando canal I2S...");
    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));

    volumen = (EXAMPLE_VOICE_VOLUME > VOLUMEN_MAXIMO_USUARIO) ? VOLUMEN_MAXIMO_USUARIO : EXAMPLE_VOICE_VOLUME;
    current_track_mutex = xSemaphoreCreateMutex();
    volume_mutex = xSemaphoreCreateMutex();
    if (volume_mutex == NULL) {
        ESP_LOGE("Audio", "No se pudo crear el mutex de volumen");
    }
    
    audio_event_queue = xQueueCreate(8, sizeof(audio_cmd_t));
    xTaskCreate(task_audio_commands, "task_audio_player", 4096, NULL, 7, NULL);
    xTaskCreate(task_audio_player, "task_audio_player", 4096, NULL, 6, NULL);

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

const char *audio_state_to_str(audio_state_t estado) {
    switch (estado) {
        case PLAYER_STOPPED: return "detenido";
        case PLAYER_PLAYING: return "reproduciendo";
        case PLAYER_PAUSED:  return "pausado";
        default: return "desconocido";
    }
}

int audio_player_get_volume(void) {
    return volumen;
}

const char* audio_player_get_song(void) {
    static char nombre_simple[64];
    const char *ruta = playlist[track_get()];

    // Extrae solo el nombre del archivo (sin el path "/spiffs/")
    const char *nombre = strrchr(ruta, '/');
    if (nombre) {
        strncpy(nombre_simple, nombre + 1, sizeof(nombre_simple) - 1);
        nombre_simple[sizeof(nombre_simple) - 1] = '\0';
        return nombre_simple;
    } else {
        return "Desconocida";
    }
}