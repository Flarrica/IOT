idf_component_register(
    SRCS "src/logger.c"
    INCLUDE_DIRS "include"
    REQUIRES nvs_flash task_mqtt
)