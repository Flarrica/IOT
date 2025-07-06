set(component_srcs "src/wifi_APSTA.c")

idf_component_register(SRCS "src/wifi_APSTA.c"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_wifi esp_event esp_netif nvs_flash task_mqtt ntp shared_lib)