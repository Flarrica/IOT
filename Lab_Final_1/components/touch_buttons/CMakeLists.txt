idf_component_register(SRCS "touch_buttons.c"
                       INCLUDE_DIRS "include"
                       REQUIRES driver led_rgb audio_player shared_lib)

# Añadir flags de compilación personalizados para este componente
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-deprecated-declarations)