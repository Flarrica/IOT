#include <stdio.h>
#include <math.h>

int Contador(char *puntero);

int main()
{
    // variables
    char texto[]=" Hola mundo ";
    int vocales = 0;
    int consonantes = 0;

    // función
    int largo = Contador(texto);

    //Mostrar si funcionó
    printf("La longitud del string es de %d caracteres. \n",largo);

    return 0;
}

int Contador(char *puntero){
    int cont = 0;

    while (*puntero != '\0')
    {
        cont++;
        puntero++;     
    }
    
return cont;
}