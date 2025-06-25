idf_component_register(
    SRCS "app_main.c"
    INCLUDE_DIRS "."
    REQUIRES
        nvs_flash
        esp_event
        esp_netif
        mqtt
        protocol_examples_common
        led_rgb
        shared_lib
        task_a
        task_b
        task_c
)