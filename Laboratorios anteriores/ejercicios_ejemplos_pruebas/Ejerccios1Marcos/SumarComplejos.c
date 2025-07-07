
#include <stdio.h>

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

int main() {
    complex_t num1 = {3.0, 2.0};  // 3 + 2i
    complex_t num2 = {1.5, -4.0}; // 1.5 - 4i
    
    complex_t resultado = sum(num1, num2);
    printf("\n------------------------------------------------\n");
    printf("\nEl valor de la suma compleja es: %.2f + %.2fi\n", resultado.real, resultado.imag);
    printf("\n------------------------------------------------\n");

    return 0;
}
