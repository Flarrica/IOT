set(component_srcs "src/task_a.c")

idf_component_register(SRCS "src/task_a.c"
                       INCLUDE_DIRS "include"
                       REQUIRES driver log ejemplo_led_strip)

