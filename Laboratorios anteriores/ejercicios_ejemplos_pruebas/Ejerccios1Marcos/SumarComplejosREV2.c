#include <stdio.h>
// Se crea una estructura para realziar la suma compleja con los componentes real e imaginario

typedef struct {
    double real;
    double imag;
} complex_t;

complex_t sum(complex_t a, complex_t b) {
    complex_t result;
    result.real = a.real + b.real;
    result.imag = a.imag + b.imag;
    return result;
}

// Función que se encarga de todo: Se pide el valor por consola, se sumar y se muestra el resultado
void procesarSumaCompleja() {
    complex_t num1, num2, resultado;
    // Se pide por consola los valores del primer numero complejo, parte real y imaginario
    printf("\n--------------------------------------------------\n");
    printf("Ingrese la parte real del primer número complejo: ");
    scanf("%lf", &num1.real);
    printf("Ingrese la parte imaginaria del primer número complejo: ");
    scanf("%lf", &num1.imag);

    printf("\n--------------------------------------------------\n");
    
    // Se pide por consola los valores del primer numero complejo, parte real y imaginario
    printf("Ingrese la parte real del segundo número complejo: ");
    scanf("%lf", &num2.real);
    printf("Ingrese la parte imaginaria del segundo número complejo: ");
    scanf("%lf", &num2.imag);
    printf("\n--------------------------------------------------\n");

    resultado = sum(num1, num2);

    // Se muestra el valor de la suma
    printf("\n------------------------------------------------\n");
    printf("El valor de la suma compleja es: %.2f + %.2fi\n", resultado.real, resultado.imag);
    printf("------------------------------------------------\n");
}

int main() {
    procesarSumaCompleja();
    return 0;
}
