#include <stdio.h>

int find_in_string() {
    char haystack[] = "Encontrar la subcadena dentro de una cadena ";
    char needle[] = "dentro";
    
   
    // Se comienza recorriendo el haystacck para intentar hacer coincidir con la subcadena.
    for (char *h = haystack; *h != '\0'; h++) 
        {
        char *h_ptr = h;
        char *n_ptr = needle;

        
        while (*h_ptr == *n_ptr && *n_ptr != '\0') {
            h_ptr++;
            n_ptr++;
        }

        // Aca cunado se encuentra la subcadea mestra la posicion en donde la encuentra Si se ha encontrado la subcadena, retornar la posición
        if (*n_ptr == '\0') 
        {
            printf("Subcadena encontrada en la posición: %ld\n", h - haystack);
            return h - haystack;  
        }
    }

    printf("Subcadena no encontrada.\n");
    return -1;
}

int main() {
    find_in_string();  
    return 0;
}
