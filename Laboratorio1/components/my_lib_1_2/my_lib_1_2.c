#include "my_lib_1_2.h"

void  init_lab(void);


void  init_lab() {
    char nombre1[] ="Fernando Fabian Larrica Gastan";
    char nombre2[] = "Gaston Diego Massaferro";
    char nombre3[] = "Marcos Giombini";

    printf("Laboratorio lenguaje C de %s, %s y %s.\n", nombre3,nombre2,nombre1);
}

root_t eq_solver(coeff_t *coeficientes){
    root_t resultado;
    double a = coeficientes->a;
    double b = coeficientes->b;
    double c = coeficientes->c;

    double determinante = b * b - 4 * a * c;

    /***********************************************************
    determinante mayor a 0 es "normal"
    ************************************************************/

    if (determinante > 0){
        resultado.R1 = (-b + sqrt(determinante)) / (2 * a);
        resultado.R2 = (-b - sqrt(determinante)) / (2 * a);
        printf("Las soluciones son: x1 = %.2f y x2 = %.2f\n", resultado.R1, resultado.R2);
    }

    /***********************************************************
    determinante igual a 0 tiene raices dobles
    ************************************************************/

    else if (determinante == 0) {
        resultado.R1 = -b / (2 * a);
        printf("La solución es una raíz doble: x = %.2f\n", resultado.R1);
    } 

    /***********************************************************
    determinante menor a 0 tiene raices complejas
    ************************************************************/

    else {
        resultado.R1 = -b / (2 * a);
        resultado.R2 = sqrt(-determinante) / (2 * a); // tomo R2 como parte imaginaria para no romper la escructura
        printf("Las soluciones son complejas:\n");
        printf("x1 = %.2f + %.2fi\n", resultado.R1, resultado.R2);
        printf("x2 = %.2f - %.2fi\n", resultado.R1, resultado.R2);
    }
    return resultado;
}