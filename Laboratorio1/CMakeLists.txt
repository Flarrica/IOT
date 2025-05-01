# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(Laboratorio1)

set(component_lib "components")
set(include_lib "include")
set(Global_lib "Global")
set(led_strip "my_led_strip")
set(led_RGB "my_led_RGB")
set(led_delay "my_led_delay")

# Agregar subdirectorios de las librerías
add_subdirectory("${component_lib}/${Global_lib}")
add_subdirectory("${component_lib}/${led_strip}")
add_subdirectory("${component_lib}/${led_RGB}")
add_subdirectory("${component_lib}/${led_delay}")

# Definir el ejecutable principal
add_executable(mi_programa src/main.c)

# Vincular las bibliotecas al ejecutable
target_link_libraries(mi_programa "${Global_lib}" "${led_strip}" "${led_RGB}" "${led_delay}")

# Agregar los directorios de encabezados de las bibliotecas
target_include_directories(mi_programa PRIVATE 
    "${component_lib}/${Global_lib}/${include_lib}"
    "${component_lib}/${led_strip}/${include_lib}"
    "${component_lib}/${led_RGB}/${include_lib}"
    "${component_lib}/${led_delay}/${include_lib}"
)

