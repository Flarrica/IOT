#include <stdio.h>
#include <math.h>

int main()
{
    int numero;
    int resto;
    int aux = 0;

    printf("ingrese un numero para chequear que sea perfecto, \n");
    printf("ingrese numero: \n");
    scanf("%d", &numero);
    
    for (int cont = 1; cont < numero; cont++){
        resto = numero % cont;
        if (resto == 0){
            aux = aux + cont;
        }
        
    }
    
    if (aux == numero){
        printf("El numero %d es un numero PERFECTO! ",numero);
    } else{
        printf("El numero %d no es un numero PERFECTO",numero);
    }
    
    return 0;
}
