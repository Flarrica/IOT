idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES led_rgb uart_manager task_a task_b task_c driver ejemplo_led_strip
)