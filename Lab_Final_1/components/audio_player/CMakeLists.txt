idf_component_register(
    SRCS "src/audio_player.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES es8311 driver
    EMBED_FILES "assets/Sonido3.pcm"
)