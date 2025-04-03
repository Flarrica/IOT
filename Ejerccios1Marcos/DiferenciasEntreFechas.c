
#include <stdio.h>
#include <time.h>

typedef struct {
    int dias;
    int mes;
    int año;
} date_t;

// Función para calcular la diferencia en días entre dos fechas
int days_left(date_t start, date_t finish) {
    struct tm tm_start = {0}, tm_finish = {0};

    // Aca se configura la estructura para la fecha start
    tm_start.tm_mday = start.dias;
    tm_start.tm_mon = start.mes - 1; // tm_mon va de 0 a 11
    tm_start.tm_year = start.año - 1900; // tm_year cuenta desde 1900

    // Configurar la estructura de fecha para finish
    tm_finish.tm_mday = finish.dias;
    tm_finish.tm_mon = finish.mes - 1;
    tm_finish.tm_year = finish.año - 1900;

    // Convertir a tiempo en segundos
    time_t time_start = mktime(&tm_start);
    time_t time_finish = mktime(&tm_finish);

    // Calcular la diferencia en días
    double difference = difftime(time_finish, time_start) / (60 * 60 * 24);

    return (int)difference;
}

// Función para ejecutar la prueba
void execute_days_left() {
    date_t fecha1 = {10, 3, 2024}; // 10 de marzo de 2024
    date_t fecha2 = {25, 3, 2025}; // 25 de marzo de 2024

    int dias = days_left(fecha1, fecha2);
    printf("\n------------------------------------------------\n");
    printf("\nDiferencia en días: %d\n", dias);
    printf("\n------------------------------------------------\n");
}

int main() {
    execute_days_left();
    return 0;
}
