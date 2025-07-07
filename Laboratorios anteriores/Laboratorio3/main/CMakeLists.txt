idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES shared_lib led_rgb task_a task_b task_c driver ejemplo_led_strip
)