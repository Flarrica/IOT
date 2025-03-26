/*********************************************************************************
Escribe un programa en C para encontrar los elementos máximo y mínimo en un array. 
*********************************************************************************/
#include <stdio.h>
#include <math.h>

int main(){
    int algo[] = {25, 10, 80, 30, 40, 5};
    int tamano = sizeof(algo) / sizeof(algo[0]);
    int max = algo[0];
    int min = algo[0];

    for (int i = 1; i < tamano; i++){
        if (max < algo[i]){
            max = algo[i];
        }
        if (min > algo[i]){
            min = algo[i];
        }
        
    }

    printf("El numero mas grande del array es: %d y el numero mas chico es: %d. \n",max ,min);
    return 0;
}
