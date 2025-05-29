#include "touch_buttons.h"
#include "driver/touch_pad.h"
#include "esp_log.h"
#include "led_rgb.h"
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"  // para usar esp_rom_delay_us()
#include <inttypes.h> 

#define TOUCH_BUTTON_NUM 6
#define TOUCH_THRESHOLD 60000

static const touch_pad_t botones_touch[TOUCH_BUTTON_NUM] = {
    TOUCH_PAD_NUM1,  // VOL_UP
    TOUCH_PAD_NUM6,  // PHOTO
    TOUCH_PAD_NUM5,  // NETWORK
    TOUCH_PAD_NUM4,  // RECORD
    TOUCH_PAD_NUM3,  // PLAY/PAUSE
    TOUCH_PAD_NUM2   // VOL_DOWN
};

static const led_rgb_evento_t color_touch[TOUCH_BUTTON_NUM] = {
    LED_EVENT_VERDE,
    LED_EVENT_ROJO,
    LED_EVENT_CIAN,
    LED_EVENT_AMARILLO,
    LED_EVENT_BLANCO,
    LED_EVENT_AZUL
};

void touch_polling_init(void) {
    touch_pad_init();
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        touch_pad_config(botones_touch[i]);
    }
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_fsm_start();
}

void touch_polling_bucle(void) {
    static uint64_t last_pressed = -1;
    uint32_t touch_value;

    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        touch_pad_read_raw_data(botones_touch[i], &touch_value);
        if (touch_value > TOUCH_THRESHOLD && last_pressed != i) {
            led_rgb_set_event(color_touch[i]);
            last_pressed = i;
        }
    }
}