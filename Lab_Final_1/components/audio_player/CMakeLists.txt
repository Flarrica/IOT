set(component_srcs "src/audio_player.c")

idf_component_register(
    SRCS "src/audio_player.c"
    INCLUDE_DIRS "include"
    REQUIRES freertos spiffs driver es8311 led_rgb shared_lib
)
