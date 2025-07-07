#include <stdio.h>

int find_in_string(char *haystack, char *needle) {
    // Si la subcadena needle es vacía, no hay nada que buscar
    if (*needle == '\0') {
        return 0;  // Retorna 0 si la subcadena es vacía
    }
    
    // Iterar sobre la cadena haystack
    for (int i = 0; haystack[i] != '\0'; i++) {
        int j = 0;
        
        // Comprobar si la subcadena needle empieza en haystack[i]
        while (haystack[i + j] == needle[j]) {
            j++;
            // Si hemos alcanzado el final de needle, entonces lo hemos encontrado
            if (needle[j] == '\0') {
                return i;  // Retorna el índice donde comienza la subcadena
            }
        }
    }
    
    // Si no se encuentra la subcadena se retorna -1
    return -1;
}

void solicitar_texto() {
    char haystack[100];
    char needle[100];
    
    //Se piden al usuario las dos cadenas haystack y needle
    printf("\n--------------------------------------------------------\n");
    printf("Ingrese la cadena principal (haystack): \n");
    fgets(haystack, 100, stdin);
    printf("Ingrese la subcadena a buscar (needle): \n");
    fgets(needle, 100, stdin);
    printf("\n--------------------------------------------------------\n");
    // Se eliminan saltos de línea cuando se hace enter, si los hay
    for (int i = 0; haystack[i] != '\0'; i++) {
        if (haystack[i] == '\n') {
            haystack[i] = '\0';
            break;
        }
    }

    for (int i = 0; needle[i] != '\0'; i++) {
        if (needle[i] == '\n') {
            needle[i] = '\0';
            break;
        }
    }

    // Se llama a la función find_in_string para encontrar la subcadena needle
    int result = find_in_string(haystack, needle);
    
    if (result != -1) {
        printf("\n--------------------------------------------------------\n");
        printf("La subcadena fue encontrada en el índice: %d\n", result);
        printf("\n--------------------------------------------------------\n");
    } else {
        printf("\n--------------------------------------------------------\n");
        printf("La subcadena no fue encontrada.\n");
        printf("\n--------------------------------------------------------\n");
    }
}

int main() {
    solicitar_texto();  
    
    return 0;
}
