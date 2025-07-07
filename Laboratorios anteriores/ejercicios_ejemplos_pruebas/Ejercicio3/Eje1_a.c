#include <stdio.h>
#include <math.h>

void cambiar(int *a, int *b);

int main()
{
    int muestra1[3]={1,2,30};
    int muestra2[3]={0,0,0};
    for(int i = 0; i < 3; i++) {
        printf("La resta de las muestra1 en la posicion %d es: %d.\n", i, muestra1[i]);
        printf("La resta de las muestra2 en la posicion %d es: %d.\n", i, muestra2[i]);
    }

    cambiar(muestra1,muestra2);
    printf("Las matrices despues del cambio quedan como: \n");
    for(int i = 0; i < 3; i++) {
            printf("La resta de las matrices en la posicion %d es: %d.\n", i, muestra1[i]);
            printf("La resta de las matrices en la posicion %d es: %d.\n", i, muestra2[i]);
    }
    return 0;
}

void cambiar(int *a, int *b){

    int aux1[3] = {0,0,0};
    int aux2[3] = {0,0,0};

    for(int i = 0; i < 3; i++) {
        aux1[i]= a[i];
        aux2[i]= b[i];
        b[i]= aux1[i];
        a[i]= aux2[i];
}


}