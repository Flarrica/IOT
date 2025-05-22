# Indicar dónde están tus archivos fuente
idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_timer esp_rom esp_wifi esp_event esp_netif nvs_flash log
)
