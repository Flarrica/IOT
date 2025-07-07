
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "es8311.h"

extern const uint8_t audio_start[] asm("_binary_audio_example_pcm_start");
extern const uint8_t audio_end[]   asm("_binary_audio_example_pcm_end");

void app_main(void)
{
    // Inicialización de canal I2S TX
    i2s_chan_handle_t tx_chan;
    i2s_channel_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_new_channel(&chan_cfg, &tx_chan, NULL);

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = GPIO_NUM_0,  // Actualizar según pines de la Kaluga
            .bclk = GPIO_NUM_1,
            .ws = GPIO_NUM_2,
            .dout = GPIO_NUM_3,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    i2s_channel_init_std(tx_chan, &std_cfg);

    // Inicialización del ES8311
    es8311_handle_t es8311_dev = es8311_create(I2C_NUM_0, ES8311_ADDRRES_111);
    es8311_init(es8311_dev);
    es8311_config_t es8311_cfg = {
        .mic_input_mode = ES8311_INPUT_LINE1,
        .dac_output_mode = ES8311_OUTPUT_HEADPHONES,
        .i2s_mode = ES8311_I2S_MASTER,
    };
    es8311_config(es8311_dev, &es8311_cfg);
    es8311_start(es8311_dev);

    size_t audio_len = audio_end - audio_start;
    size_t written = 0;
    while (true) {
        i2s_channel_write(tx_chan, audio_start, audio_len, &written, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
