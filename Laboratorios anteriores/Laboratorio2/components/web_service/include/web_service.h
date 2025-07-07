#ifndef WEB_SERVICE_H
#define WEB_SERVICE_H

extern int led_mode_web;  // esta variable se modifica desde web_service.c
void web_service_inicializar(void);
void web_service_bucle(void);

#endif