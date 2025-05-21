set(component_srcs "src/led_RGB_control.c")

idf_component_register(SRCS "src/led_RGB_control.c"
                       INCLUDE_DIRS "include"
                       REQUIRES ejemplo_led_strip driver)

