#ifndef WIFI_APSTA_H
#define WIFI_APSTA_H
#include <stdbool.h>

void wifi_apsta_inicializar(void);
void wifi_apsta_bucle(void);
bool wifi_sta_conectado(void);

#endif