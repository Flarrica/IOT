#ifndef AUDIO_PLAYER_H_
#define AUDIO_PLAYER_H_

#include "esp_err.h"

// Definifmos estos comandos de forma similar a los LEDs
typedef enum {
    AUDIO_CMD_PLAY,
    AUDIO_CMD_STOP,
    AUDIO_CMD_NEXT,
    AUDIO_CMD_PREV,
    AUDIO_CMD_VOL_UP,
    AUDIO_CMD_VOL_DOWN
} audio_cmd_t;

// Definifmos tipo evento audio, similar a los eventos LED
typedef struct {
    audio_cmd_t cmd;
} audio_event_t;

esp_err_t audio_player_init(void);
esp_err_t audio_player_play_file(const char *filepath);
esp_err_t audio_player_enqueue(audio_event_t evt);
void task_audio_player(void *pvParams);


#endif // AUDIO_PLAYER_H_