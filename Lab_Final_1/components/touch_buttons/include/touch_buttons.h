#ifndef TOUCH_BUTTONS_H
#define TOUCH_BUTTONS_H
#include "driver/touch_pad.h"

void touch_polling_init(void);
void touch_polling_bucle(void);
void task_touch(void *pvParameters);

#endif // TOUCH_BUTTONS_H