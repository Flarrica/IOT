#include <stdio.h>

void tring_copy() {
    char origen[] = "\nSe copia un string de origen a destino!";
    char destino[50]; 

   /*Usando punteros como el ejemplo visto en clase se copia caracter por caracter recorriendo los lugares del
   tring origen copiandolo al destino*/
    char *ptr_origen = origen;
    char *ptr_destino = destino;

    while (*ptr_origen != '\0') {
        *ptr_destino = *ptr_origen;
        ptr_origen++;
        ptr_destino++;
    }
    *ptr_destino = '\0';  
     printf("\n----------------------------------------------");
     printf("\nEl string copiado es: %s\n", destino);
     printf("\n----------------------------------------------");
}

int main() {
    tring_copy(); 
    return 0;
}
