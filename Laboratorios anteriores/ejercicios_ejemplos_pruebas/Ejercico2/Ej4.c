#include <stdio.h>
#include <math.h>

void SegGrado( float a, float b, float c, float *raiz1, float *raiz2);

int main()
{
    float param1 = 1;
    float param2 = -5;
    float param3 = 6;

    float raiz1, raiz2;

    SegGrado(param1,param2,param3,&raiz1, &raiz2);
    printf("las raices son %.2f y %.2f.",raiz1,raiz2);
    
    return 0;
}

void SegGrado( float a, float b, float c, float *raiz1, float *raiz2)
{
    float determinante;

    /***********************************************************
    calculo el determinante y las raices
    ************************************************************/
    
    determinante = b * b - 4 * a * c;

    /***********************************************************
    determinante mayor a 0 es "normal"
    ************************************************************/

    if (determinante > 0){
        *raiz1 = (-b + sqrt(determinante)) / (2 * a);
        *raiz2 = (-b - sqrt(determinante)) / (2 * a);
    }

    /***********************************************************
    determinante igual a 0 tiene raices dobles
    ************************************************************/

    else {
        *raiz1 = -b / (2 * a);
        *raiz2 = *raiz1;
    } 
}