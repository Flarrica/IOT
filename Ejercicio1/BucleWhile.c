#include <stdio.h>
#include <math.h>

int main(int argc, char const *argv[])
{
    int numero;
    int prueba;
    int inverso;

    printf("Ingrese un numero: \n");
    scanf("%d", &numero);
    prueba = numero;
    

    while (prueba > 0){
        inverso = (inverso * 10) + prueba % 10;
        prueba = prueba / 10;
        // prueba /= 10; Otra forma de escribir lo de arriba
    }

    if (inverso == numero){
        printf("El numero es un palindromo! \n");
    } else{
        printf("El numero no es un palindromo. \n");
    }
    
    return 0;
}
