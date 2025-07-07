#include <stdio.h>
#include <math.h>

int main()
{
    int lado1;
    int lado2;
    int lado3;
    printf("ingrese las medidas de los lados del triangulo, \n");
    printf("ingrese a: \n");
    scanf("%d", &lado1);
    printf("ingrese b: \n");
    scanf("%d", &lado2);
    printf("ingrese c: \n");
    scanf("%d", &lado3);
    
    if (lado1 == lado2 && lado2 == lado3 && lado1 == lado3){
        printf("Es un triangulo Equilatero. \n");
    }else if (lado1 == lado2 || lado2 == lado3 || lado1 == lado3){
        printf("Es un triangulo Isosceles. \n");
    }else{
        printf("Es un triangulo Escaleno. \n");
    }
    
    return 0;
} 
