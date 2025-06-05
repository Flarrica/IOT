set(component_srcs "src/task_c.c")

idf_component_register(
                        SRCS "src/task_c.c"
                        INCLUDE_DIRS "include"
                        REQUIRES led_rgb uart_manager freertos
                    )

