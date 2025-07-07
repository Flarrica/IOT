#ifndef MY_LIB_1_1_H_
#define MY_LIB_1_1_H_


int contieneNumero(char *texto);
void string_words(char *string);

void solicitarTexto();
void string_to_caps(char *string);
void string_to_min(char *string);


int string_copy(char *source, char *destination);

typedef struct {
    int dias;  // se definen dias del mes (1 31)
    int mes;   // se definen mes del año (1 a 12)
    int anio;   // se definen los Años (ej: 2025)
} date_t;
int days_left(date_t start, date_t finish);
void ejecutarCalculoDeDias();

void limpiar_buffer();
int find_in_string(char *haystack, char *needle);
void solicitar_texto();

//Multiplicacion dos numeros complejos
typedef struct {
    double real;
    double imag;
} complexf_t;
complexf_t prod(complexf_t a, complexf_t b);
void procesarMultiplicacionCompleja(); 



// Suma dos numeros complejos
typedef struct {
    double real;
    double imag;
} complex_t;

complex_t sum(complex_t a, complex_t b);
void procesarSumaCompleja();


#endif
