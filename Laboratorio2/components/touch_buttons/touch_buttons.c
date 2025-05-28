#include "touch_buttons.h"
#include "touch_element/touch_button.h"
#include "esp_log.h"
#include "esp_rom_sys.h"  // Para esp_rom_delay_us()


#define TOUCH_BUTTON_NUM 14

static const char *TAG = "Touch Button";
static touch_button_handle_t button_handle[TOUCH_BUTTON_NUM];

static const touch_pad_t channel_array[TOUCH_BUTTON_NUM] = {
    TOUCH_PAD_NUM1, 
    TOUCH_PAD_NUM2, 
    TOUCH_PAD_NUM3, 
    TOUCH_PAD_NUM4,
    TOUCH_PAD_NUM5, 
    TOUCH_PAD_NUM6, 
    TOUCH_PAD_NUM7, 
    TOUCH_PAD_NUM8,
    TOUCH_PAD_NUM9, 
    TOUCH_PAD_NUM10, 
    TOUCH_PAD_NUM11, 
    TOUCH_PAD_NUM12,
    TOUCH_PAD_NUM13, 
    TOUCH_PAD_NUM14,
};

static const float channel_sens_array[TOUCH_BUTTON_NUM] = {
    0.1F, 
    0.1F, 
    0.1F, 
    0.1F, 
    0.1F, 
    0.1F, 
    0.1F,
    0.1F,
    0.1F, 
    0.1F, 
    0.1F, 
    0.1F, 
    0.1F, 
    0.1F,
};

static void button_handler(touch_button_handle_t out_handle, touch_button_message_t *out_message, void *arg)
{
    int btn = (int)arg;
    switch (out_message->event) {
        case TOUCH_BUTTON_EVT_ON_PRESS:
            ESP_LOGI(TAG, "Button[%d] Press", btn);
            break;
        case TOUCH_BUTTON_EVT_ON_RELEASE:
            ESP_LOGI(TAG, "Button[%d] Release", btn);
            break;
        case TOUCH_BUTTON_EVT_ON_LONGPRESS:
            ESP_LOGI(TAG, "Button[%d] LongPress", btn);
            break;
        default:
            break;
    }
}

void touch_buttons_init(void)
{
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(touch_element_install(&global_config));
    ESP_LOGI(TAG, "Touch element library installed");

    touch_button_global_config_t button_global_config = TOUCH_BUTTON_GLOBAL_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(touch_button_install(&button_global_config));
    ESP_LOGI(TAG, "Touch button installed");

    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        touch_button_config_t button_config = {
            .channel_num = channel_array[i],
            .channel_sens = channel_sens_array[i]
        };
        ESP_ERROR_CHECK(touch_button_create(&button_config, &button_handle[i]));
        ESP_ERROR_CHECK(touch_button_subscribe_event(button_handle[i],
            TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE | TOUCH_ELEM_EVENT_ON_LONGPRESS,
            (void *)channel_array[i]));
        ESP_ERROR_CHECK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_CALLBACK));
        ESP_ERROR_CHECK(touch_button_set_callback(button_handle[i], button_handler));
        ESP_ERROR_CHECK(touch_button_set_longpress(button_handle[i], 2000));
    }

    ESP_LOGI(TAG, "Touch buttons created");
    touch_element_start();
    ESP_LOGI(TAG, "Touch element library start");
}