#include <stdio.h>
#include <math.h>
#include <string.h>

void Inversor(char *puntero);

int main()
{
    // variables
    char texto[]="Hola mundo";

    // función
    Inversor(texto);

    //Mostrar si funcionó
    printf("Texto invertido: %s\n", texto);

    return 0;
}

void Inversor(char *puntero){
    int largo = strlen(puntero);
    printf("El largo de la cadena de texto es: %d. \n", largo);

    char aux;
    int inicio = 0;
    int final = largo-1; // agregar -1 xq sino el text comienza con \n y no lo muestra 

    while (inicio < final){
        aux = puntero[inicio];
        puntero[inicio] = puntero[final];
        puntero[final] = aux;

        inicio++;
        final--;
    }
    
}