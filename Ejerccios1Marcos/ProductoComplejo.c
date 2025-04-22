

#include <stdio.h>

typedef struct {
    double real;
    double imag;
} complex_t;

complex_t prod(complex_t a, complex_t b) {
    complex_t result;

    /* Para hacer la multiplicacion en complejo parto de esta ecuacion
     (a+bi)×(c+di)=(ac−bd)+(ad+bc)i
     */
   
     result.real = (a.real * b.real) - (a.imag * b.imag);
     result.imag = (a.real * b.imag) + (a.imag * b.real);
    return result;
    
}

int main() {
    complex_t num1 = {3.0, 2.0};  // 3 + 2i
    complex_t num2 = {1.5, -4.0}; // 1.5 - 4i
    
    complex_t resultado = prod(num1, num2);
    printf("\n------------------------------------------------\n");
    printf("\nEl valor de la multiplicacion compleja es: %.2f + %.2fi\n", resultado.real, resultado.imag);
    printf("\n------------------------------------------------\n");

    return 0;
}
