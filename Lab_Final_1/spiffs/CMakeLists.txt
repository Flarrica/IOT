#idf_component_register(SRCS "")
#spiffs_create_partition_image(spiffs ../spiffs FLASH_IN_PROJECT)

# No se compila ningún archivo fuente para este "componente"
idf_component_register(SRCS "" INCLUDE_DIRS "")

# Generar imagen SPIFFS a partir del directorio real
spiffs_create_partition_image(spiffs "${CMAKE_SOURCE_DIR}/spiffs" FLASH_IN_PROJECT)