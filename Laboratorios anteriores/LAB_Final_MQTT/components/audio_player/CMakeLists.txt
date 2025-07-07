idf_component_register(
    SRCS "src/audio_player.c"
    INCLUDE_DIRS "include"
    REQUIRES es8311 driver spiffs
)