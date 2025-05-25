#include <stdio.h>
#include <stdbool.h>
#include "global.h"


//static bool led_on = false;

void app_main(void)
{
    touch_buttons_init();

    while (1) {
        esp_rom_delay_us(100000); // 100 ms
    }

}