idf_component_register(SRCS "src/task_mqtt.c"
                       INCLUDE_DIRS "include"
                       REQUIRES mqtt esp_wifi json audio_player)