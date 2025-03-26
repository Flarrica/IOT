/************************************************************************************************************
Escribe un programa en C para leer n número de valores en un array y mostrarlos en pantalla en orden inverso. 
************************************************************************************************************/

#include <stdio.h>
#include <math.h>
#define MaxLoop 5

int main()
{
    // int algo[MaxLoop];
    int algo[] = {25, 10, 20, 30, 40, 50};
    int inversoAlgo[MaxLoop];
    int cont = 0;

    /************************************************ 
    // El siguiente codigo se puede desmarcar y crea un array que depende de MaxLoop
    //Comentar array algo[] de arriba 

    for (int i = 0; i < MaxLoop+1; i++){
        algo[i]=i;
        printf("%d \n", algo[i]);
    }

    *************************************************/
    for (int i = MaxLoop; i+1 > 0; i--){ 
        inversoAlgo[i]=algo[cont];
        printf("En el lugar %d del array inverso se encuentrael valor: %d \n", i, inversoAlgo[i]);
        cont++;
        
    }
    
    return 0;
}
