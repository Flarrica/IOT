set(component_srcs "src/task_b.c")

idf_component_register(
                        SRCS "src/task_b.c"
                        INCLUDE_DIRS "include"
                        REQUIRES led_rgb uart_manager freertos
                    )

