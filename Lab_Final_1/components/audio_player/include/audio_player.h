#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "esp_err.h"

/* Configuraciones del reproductor de audio */

#define EXAMPLE_RECV_BUF_SIZE    (2400)
#define EXAMPLE_SAMPLE_RATE      (16000)
#define EXAMPLE_MCLK_MULTIPLE    (256) // 256 es suficiente si no se usa 24 bits por muestra
#define EXAMPLE_MCLK_FREQ_HZ     (EXAMPLE_SAMPLE_RATE * EXAMPLE_MCLK_MULTIPLE)
#define EXAMPLE_VOICE_VOLUME     80

// Pin de control del amplificador (PA - Power Amplifier)
#define EXAMPLE_PA_CTRL_IO       (GPIO_NUM_10)

#if defined(CONFIG_EXAMPLE_MODE_ECHO)
#define EXAMPLE_MIC_GAIN         CONFIG_EXAMPLE_MIC_GAIN
#endif

#if !defined(CONFIG_EXAMPLE_BSP)

/* Configuración de I2C */
#define I2C_NUM      (0)
#define I2C_SCL_IO   (GPIO_NUM_7)
#define I2C_SDA_IO   (GPIO_NUM_8)

/* Configuración de I2S */
#define I2S_NUM      (1)
#define I2S_DO_IO    (GPIO_NUM_12)
#define I2S_DI_IO    (GPIO_NUM_46)
#define I2S_MCK_IO   (GPIO_NUM_35)
#define I2S_BCK_IO   (GPIO_NUM_18)
#define I2S_WS_IO    (GPIO_NUM_17)

#endif /* !CONFIG_EXAMPLE_BSP */

/* Declaraciones públicas del reproductor de audio */
esp_err_t audio_player_init(void);
esp_err_t audio_player_start(void);

#endif // AUDIO_PLAYER_H