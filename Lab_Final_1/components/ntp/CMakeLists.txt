set(component_srcs "src/ntp.c")

idf_component_register(
    SRCS "src/ntp.c"
    INCLUDE_DIRS "include"
    REQUIRES lwip
    )
