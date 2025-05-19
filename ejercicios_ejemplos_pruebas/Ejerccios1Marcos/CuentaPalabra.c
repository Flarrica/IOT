
#include <stdio.h>

// Se crea una función que cuenta palabras. Primero se verifica que el string no sea vacío
void string_words() {
    
    char texto[] = "Este es un ejemplo para contar las palabras";
    char *ptr = texto; 
    
    if (*texto == '\0') { 
        printf("Número de palabras: 0\n"); 
        return; 
    }

    int count = 0;
    int inPalabra = 0; 

    /* con un puntero recorremos los lugares de memoria. Con una variable inpalabra cuando pasa de (0) a  (1)
    el count aumenta su valor porque comienza una nueva palabra. Entonces cuando inPalabra es 0 esta porque hay un 
    espacio o tabulacion o un salto */
    while (*ptr) 
    {
        if (*ptr == ' ' || *ptr == '\t' || *ptr == '\n') {
            inPalabra = 0; 
        } else if (!inPalabra) {
            inPalabra = 1; 
            count++;
        }
        ptr++; 
    }

    
    printf("\n--------------------------------------------------------\n");
    printf("Texto: \"%s\"\n", texto);
    printf("Número de palabras: %d\n", count);
    printf("\n--------------------------------------------------------\n");
}

int main() {
    string_words(); 
    return 0;
}
