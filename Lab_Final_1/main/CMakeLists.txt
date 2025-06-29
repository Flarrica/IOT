idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES 
        audio_player
        esp_driver_i2s
        esp_driver_gpio
        es8311
        driver
        nvs_flash
        esp_netif
        esp_event
        task_a
        task_b
        task_c
        led_rgb
        shared_lib
        task_mqtt
        wifi_APSTA 
        web_service
)

spiffs_create_partition_image(spiffs ${CMAKE_CURRENT_SOURCE_DIR}/../spiffs FLASH_IN_PROJECT)