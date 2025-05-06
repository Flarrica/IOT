set(component_srcs "src/led_delay.c")

idf_component_register(SRCS "src/led_delay.c"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "driver"
                       REQUIRES "")

