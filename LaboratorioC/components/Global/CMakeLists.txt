add_library(Global Global.c)


# Especificar el directorio de los headers para esta librería
target_include_directories(Global PUBLIC include)

    # components/Global/CMakeLists.txt

target_include_directories(Global PRIVATE 
    "../${lib_1_1}/${include_lib}"
    "../${lib_1_2}/${include_lib}"
    "../${lib_2}/${include_lib}"
)