#include <stdio.h>     
#include <time.h>      // Para manejar fechas y horas con struct tm, mktime, difftime

// Definimos un tipo de dato personalizado para representar una fecha
typedef struct {
    int dias;  // Día del mes
    int mes;   // Mes (1 a 12)
    int año;   // Año (ej: 2025)
} date_t;

// Función que calcula la diferencia en días entre dos fechas
int days_left(date_t start, date_t finish) {
    struct tm tm_start = {0}, tm_finish = {0}; // Estructuras de tiempo inicializadas en 0

    // Cargamos los datos de la primera fecha en la estructura tm
    tm_start.tm_mday = start.dias;            // Día del mes
    tm_start.tm_mon = start.mes - 1;          // Mes (0 a 11, por eso restamos 1)
    tm_start.tm_year = start.año - 1900;      // Años desde 1900

    // Cargamos los datos de la segunda fecha
    tm_finish.tm_mday = finish.dias;
    tm_finish.tm_mon = finish.mes - 1;
    tm_finish.tm_year = finish.año - 1900;

    // Convertimos las estructuras tm a tiempo en segundos desde 1970 (epoch time)
    time_t time_start = mktime(&tm_start);
    time_t time_finish = mktime(&tm_finish);

    // Calculamos la diferencia en segundos, y luego la convertimos a días
    double difference = difftime(time_finish, time_start) / (60 * 60 * 24);

    // Devolvemos la diferencia como número entero
    return (int)difference;
}

// Esta función pide al usuario las fechas por consola, calcula la diferencia y la muestra
void ejecutarCalculoDeDias() {
    date_t fecha1, fecha2;

    // Pedimos la primera fecha al usuario
    printf("Ingrese la primera fecha (DD MM AAAA): ");
    scanf("%d %d %d", &fecha1.dias, &fecha1.mes, &fecha1.año);

    // Pedimos la segunda fecha
    printf("Ingrese la segunda fecha (DD MM AAAA): ");
    scanf("%d %d %d", &fecha2.dias, &fecha2.mes, &fecha2.año);

    // Calculamos la diferencia en días usando la función days_left
    int dias = days_left(fecha1, fecha2);

    // Mostramos el resultado al usuario en dias 
    printf("\n------------------------------------------------\n");
    printf("Diferencia en días: %d\n", dias);
    printf("------------------------------------------------\n");
}


int main() {
    ejecutarCalculoDeDias();
    return 0;
}
