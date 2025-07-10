#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "sdkconfig.h"
//#include "driver/gpio.h"
#include "esp_err.h"


/* Configuraciones del reproductor de audio */

#define EXAMPLE_RECV_BUF_SIZE    (2400)
#define EXAMPLE_SAMPLE_RATE      (8000)
#define EXAMPLE_MCLK_MULTIPLE    (256) // 256 es suficiente si no se usa 24 bits por muestra
#define EXAMPLE_MCLK_FREQ_HZ     (EXAMPLE_SAMPLE_RATE * EXAMPLE_MCLK_MULTIPLE)
#define EXAMPLE_VOICE_VOLUME     85
#define MAX_PLAYLIST 10
#define MAX_FILENAME_LEN 255
#define MAX_PATH_LEN (8 + MAX_FILENAME_LEN + 1)

// Pin de control del amplificador (PA - Power Amplifier)
#define EXAMPLE_PA_CTRL_IO       (GPIO_NUM_10)

#if defined(CONFIG_EXAMPLE_MODE_ECHO)
#define EXAMPLE_MIC_GAIN         CONFIG_EXAMPLE_MIC_GAIN
#endif

#if !defined(CONFIG_EXAMPLE_BSP)

// Configuración condicional de pin de entrada de datos I2S
#if CONFIG_KALUGA_VERSION_1_2
    #define I2S_DI_IO (GPIO_NUM_46)
#else
    #define I2S_DI_IO (GPIO_NUM_34)
#endif

/* Configuración de I2C */
#define I2C_NUM      (0)
#define I2C_SCL_IO   (GPIO_NUM_7)
#define I2C_SDA_IO   (GPIO_NUM_8)

/* Configuración de I2S */
#define I2S_NUM      (1)
#define I2S_DO_IO    (GPIO_NUM_12)
#define I2S_MCK_IO   (GPIO_NUM_35)
#define I2S_BCK_IO   (GPIO_NUM_18)
#define I2S_WS_IO    (GPIO_NUM_17)

#endif /* !CONFIG_EXAMPLE_BSP */

// Header de .WAV
typedef struct __attribute__((packed)) {
    char riff_id[4];
    uint32_t riff_size;
    char wave_id[4];
    char fmt_id[4];
    uint32_t fmt_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data_id[4];
    uint32_t data_size;
} wav_header_t;

/* Comandos de control de audio que pueden ser enviados por otras tareas */
typedef enum {
    CMD_PLAY,             // Tocar PLAY cuando el ultimo comando será CMD_PAUSE.  
    
    CMD_STOP,
    CMD_NEXT,
    CMD_PREV,

    CMD_VOL_UP,          // ↑ PUSH: sube +2
    CMD_VOL_DOWN,        // ↓ PUSH: baja -2

    CMD_PAUSE,          // Este queda por fuera del polling de botones. Botones solo reconoce play y la logica se da en la task de comandos
} audio_cmd_t;

// Estado del reproductor
typedef enum {
    PLAYER_STOPPED,
    PLAYER_PLAYING,
    PLAYER_PAUSED
} audio_state_t;

// Permitir a otras tareas consultar el estado (si querés exponerlo)
audio_state_t audio_player_get_state(void);

// Y también si querés exponer comandos externos de control:
void audio_player_send_cmd(audio_cmd_t cmd);

// Declaramos la cola de eventos de audio como extern para que puedan agregar elementos desde otras librerias
extern QueueHandle_t audio_event_queue;

/* Declaraciones públicas del reproductor de audio */
esp_err_t audio_player_init(void);

// Cargar playlist
void load_playlist_from_spiffs();

const char *audio_state_to_str(audio_state_t estado);

int audio_player_get_volume(void);
const char* audio_player_get_song(void);
bool audio_player_reproducir_archivo(const char *nombre_archivo);

#endif // AUDIO_PLAYER_H
