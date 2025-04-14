#ifndef MY_LIB_1_1_H_
#define MY_LIB_1_1_H_

//void my_lib_1_1_function();
int contieneNumero(char *texto);
void string_words();

void string_to_caps(char *string);
void string_to_min(char *string);

void solicitar_texto();
int string_copy(char *source, char *destination);

typedef struct {
    int dias;  // Día del mes
    int mes;   // Mes (1 a 12)
    int año;   // Año (ej: 2025)
} date_t;
int days_left(date_t start, date_t finish);
void ejecutarCalculoDeDias();

int find_in_string(char *haystack, char *needle);
void solicitar_texto();

//Multiplicacion dos numeros complejos
typedef struct {
    double real;
    double imag;
} complex_t;
complex_t prod(complex_t a, complex_t b);
void procesarMultiplicacionCompleja();

// Suma dos numeros complejos
typedef struct {
    double real;
    double imag;
} complex_t;

complex_t sum(complex_t a, complex_t b);
void procesarSumaCompleja();


#endif
