idf_component_register(SRCS "src/task_mqtt.c"
                       INCLUDE_DIRS "include"
                       REQUIRES mqtt json led_rgb shared_lib)