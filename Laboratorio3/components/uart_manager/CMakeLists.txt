idf_component_register(SRCS "src/uart_manager.c"
                       INCLUDE_DIRS "include"
                       REQUIRES driver freertos led_rgb
                    )