#include <stdio.h>
#include <math.h>

int main()
{
    float a,b,c;
    float determinante,raiz1,raiz2;

    /***********************************************************
    Ingreso las varibles de la ecuación
    ************************************************************/

    printf("es una ecuación de segundo grado, de la forma ax^2+bx+c. \n");
    printf("Ingrese a: \n");
    scanf("%f",&a);
    printf("Ingrese b: \n");
    scanf("%f",&b);
    printf("Ingrese c: \n");
    scanf("%f",&c);

    /***********************************************************
    a tiene q ser distinto de 0 para ser de segundo grado
    ************************************************************/

    if (a == 0) {
        printf("El valor de 'a' no puede ser 0 en una ecuación cuadrática.\n");
        return 1;
    }
    
    /***********************************************************
    calculo el determinante y las raices
    ************************************************************/
    
    determinante = b * b - 4 * a * c;

    /***********************************************************
    determinante mayor a 0 es "normal"
    ************************************************************/

    if (determinante > 0){
        raiz1 = (-b + sqrt(determinante)) / (2 * a);
        raiz2 = (-b - sqrt(determinante)) / (2 * a);
        printf("Las soluciones son: x1 = %.2f y x2 = %.2f\n", raiz1, raiz2);
    }

    /***********************************************************
    determinante igual a 0 tiene raices dobles
    ************************************************************/

    else if (determinante == 0) {
        raiz1 = -b / (2 * a);
        printf("La solución es una raíz doble: x = %.2f\n", raiz1);
    } 

    /***********************************************************
    determinante menor a 0 tiene raices complejas
    ************************************************************/

    else {
        float realParte = -b / (2 * a);
        float imaginarioParte = sqrt(-determinante) / (2 * a);
        printf("Las soluciones son complejas:\n");
        printf("x1 = %.2f + %.2fi\n", realParte, imaginarioParte);
        printf("x2 = %.2f - %.2fi\n", realParte, imaginarioParte);
    }

    return 0;
}

