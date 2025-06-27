#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "esp_err.h"

// Parámetros de configuración del audio
#define EXAMPLE_RECV_BUF_SIZE    (2400)
#define EXAMPLE_SAMPLE_RATE      (16000)
#define EXAMPLE_MCLK_MULTIPLE    (256)
#define EXAMPLE_MCLK_FREQ_HZ     (EXAMPLE_SAMPLE_RATE * EXAMPLE_MCLK_MULTIPLE)
#define EXAMPLE_VOICE_VOLUME     80

// Control del amplificador de potencia
#define EXAMPLE_PA_CTRL_IO       GPIO_NUM_10

// Pin de entrada de datos I2S según la versión de la Kaluga
#if CONFIG_KALUGA_VERSION_1_2
    #define I2S_DI_IO GPIO_NUM_46
#else
    #define I2S_DI_IO GPIO_NUM_34
#endif

// Configuración de I2C
#define I2C_NUM      0
#define I2C_SCL_IO   GPIO_NUM_7
#define I2C_SDA_IO   GPIO_NUM_8

// Configuración de I2S
#define I2S_NUM      1
#define I2S_DO_IO    GPIO_NUM_12
#define I2S_MCK_IO   GPIO_NUM_35
#define I2S_BCK_IO   GPIO_NUM_18
#define I2S_WS_IO    GPIO_NUM_17

#endif // AUDIO_PLAYER_H