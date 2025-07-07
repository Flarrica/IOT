set(component_srcs "src/delay.c")

idf_component_register(SRCS "src/delay.c"
                       INCLUDE_DIRS "include"
                       REQUIRES driver log ejemplo_led_strip)

