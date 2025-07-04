#ifndef WIFI_APSTA_H
#define WIFI_APSTA_H
#include <stdbool.h>

void wifi_apsta_inicializar(void);

void wifi_apsta_bucle(void);

void wifi_sta_task(void *param);

#endif