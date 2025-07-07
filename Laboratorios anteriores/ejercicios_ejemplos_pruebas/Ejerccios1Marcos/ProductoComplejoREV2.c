#include <stdio.h>

typedef struct {
    double real;
    double imag;
} complex_t;

// Se crea una función que realiza el producto de dos números complejos
complex_t prod(complex_t a, complex_t b) {
    complex_t result;
    result.real = (a.real * b.real) - (a.imag * b.imag);
    result.imag = (a.real * b.imag) + (a.imag * b.real);
    return result;
}

// Esta función maneja toda la lógica: Pide el numero por consola, llama a la funcion que hace la operacion, imprime el resultado de la multiplicacion
void procesarMultiplicacionCompleja() {
    complex_t num1, num2, resultado;

    printf("Ingrese la parte real e imaginaria respectivamente del primer número complejo:\n ");
    scanf("%lf %lf", &num1.real,&num1.imag);

    printf("Ingrese la parte real e imagnaria respectivamente del segundo número complejo:\n ");
    scanf("%lf %lf", &num2.real, &num2.imag);


    resultado = prod(num1, num2);

    printf("\n------------------------------------------------\n");
    printf("El resultado de la multiplicación es: %.2f + %.2fi\n", resultado.real, resultado.imag);
    printf("------------------------------------------------\n");
}

int main() {
    procesarMultiplicacionCompleja();
    return 0;
}
