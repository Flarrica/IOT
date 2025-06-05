#pragma once
#include "led_rgb.h"

typedef struct {
    led_rgb_evento_t color;
    uint32_t delay_seconds;
} uart_command_t;

void uart_init(void);
void uart_read_command(led_rgb_evento_t *color, uint32_t *delay_seconds);