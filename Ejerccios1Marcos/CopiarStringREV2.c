#include <stdio.h>

int string_copy(char *source, char *destination) {
    // Se pide texto al usuario por consola
    printf("\n--------------------------------------------------------\n");
    printf("Ingrese un texto (Con un max 50 caracteres): \n");
    fgets(source, 50, stdin);  // Se le el texto de origen desde la consola
    printf("\n--------------------------------------------------------\n");
    
    // Eliminar salto de línea utilizando el puntero
    char *ptr_source = source;  // Puntero al inicio de la cadena
    while (*ptr_source != '\0') {
        if (*ptr_source == '\n') {
            *ptr_source = '\0';  // Reemplazamos el salto de línea con '\0'
            break;
        }
        ptr_source++;  // Movemos el puntero al siguiente carácter
    }

    // Copiar el texto al destino
    int i = 0;
    while (source[i] != '\0') {
        destination[i] = source[i];
        i++;
    }
    destination[i] = '\0';  // Aca se Asegura que el destino esté correctamente terminado
    printf("\n--------------------------------------------------------\n");
    printf("Texto original es: %s\n", source);
    printf("\n--------------------------------------------------------\n");
    printf("El texto copiado es: %s\n", destination);
    printf("\n--------------------------------------------------------\n");

    return 0;  
}

int main() {
    char source[50];   //Se declara el array con 50 lugares dentro del main. esto va dentro del punto c
    char destination[50]; 
    
    string_copy(source, destination); 
    
    return 0;
}
