set(component_srcs "src/led_RGB.c")

idf_component_register(SRCS "src/led_RGB.c"
                       INCLUDE_DIRS "include"
                       REQUIRES driver log ejemplo_led_strip)

