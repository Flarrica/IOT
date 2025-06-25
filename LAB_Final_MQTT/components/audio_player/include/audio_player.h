#ifndef AUDIO_PLAYER_H_
#define AUDIO_PLAYER_H_

#include "esp_err.h"

esp_err_t audio_player_init(void);
esp_err_t audio_player_play_file(const char *filepath);


#endif // AUDIO_PLAYER_H_