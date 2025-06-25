cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(LAB_Final_MQTT)

spiffs_create_partition_image(spiffs ${CMAKE_CURRENT_SOURCE_DIR}/spiffs FLASH_IN_PROJECT)