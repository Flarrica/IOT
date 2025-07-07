set(component_srcs "src/shared_lib.c")

idf_component_register(
                        SRCS "src/shared_lib.c"
                        INCLUDE_DIRS "include"
                        REQUIRES freertos led_rgb
                    )

