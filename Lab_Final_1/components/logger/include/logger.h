// logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <time.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOGGER_EVENT_PLAY,
    LOGGER_EVENT_PAUSE,
    LOGGER_EVENT_NEXT,
    LOGGER_EVENT_PREVIOUS,
    LOGGER_EVENT_STOP
} logger_event_type_t;

#define LOGGER_EVENT_MAX 20


typedef struct {
    time_t timestamp;
    logger_event_type_t event_type;
} logger_event_t;

esp_err_t logger_init(void);
esp_err_t logger_add_event(logger_event_type_t type);
esp_err_t logger_get_all_events(logger_event_t *events, size_t max_events, size_t *actual_count);
esp_err_t logger_clear_all(void);
void logger_print_all(void);

#ifdef __cplusplus
}
#endif

#endif // LOGGER_H