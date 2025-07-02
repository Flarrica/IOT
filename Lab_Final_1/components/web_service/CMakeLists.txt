set(component_srcs "src/web_service.c")

idf_component_register(SRCS "src/web_service.c"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_http_server led_rgb audio_player)

