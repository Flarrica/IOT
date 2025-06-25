idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."

    REQUIRES
    nvs_flash
    esp_netif
    esp_event
    protocol_examples_common

    task_a
    task_b
    task_c
    led_rgb
    shared_lib
    task_mqtt
)