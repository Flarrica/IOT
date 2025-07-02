idf_component_register(
    SRCS "src/task_b.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_uart freertos shared_lib
)