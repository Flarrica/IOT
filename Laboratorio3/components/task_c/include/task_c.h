#ifndef TASK_C_H_
#define TASK_C_H_

#include <stdbool.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

#include "shared_lib.h"

#include "esp_log.h"

void task_c(void *pvParameters);

#endif // TASK_C_H_