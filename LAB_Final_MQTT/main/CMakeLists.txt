idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."

    REQUIRES
    audio_player
    es8311
    driver
    
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
    
    wifi_APSTA 
    web_service
)