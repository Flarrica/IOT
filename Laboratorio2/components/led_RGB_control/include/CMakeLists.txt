set(component_srcs "src/led_rgb.c")

idf_component_register(SRCS "src/led_rgb.c"
                       INCLUDE_DIRS "include"
                       REQUIRES ejemplo_led_strip driver)

