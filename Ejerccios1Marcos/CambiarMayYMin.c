#include <stdio.h>

void string_to_may(char *string) {
    while (*string) {
        /*
        Aca se verifica con la tabla ASCII si el *estring es mayor a 97 y menor que 122 se le reste 32 para que 
        tomo el valor de la mayuscula en ASCII. El valor del puntero toma el nuevo valor del caracter en mayuscula
        */
        if (*string >= 'a' && *string <= 'z') {
            
            *string = *string - ('a' - 'A');
        }
        string++;
    }
}

void string_to_min(char *string) {
    while (*string) 
    {
       /*
       Aca se verifica con la tabla ASCII si el *estring es mayor a 65 y menor que 90 se le suma 32 para que 
        tome el valor de la miniscula en ASCII. El valor del puntero toma el nuevo valor del caracter en miniscula

       */
        if (*string >= 'A' && *string <= 'Z') {
            
            *string = *string + ('a' - 'A');
        }
        string++;
    }
}

int main() {
    
    char str1[] = "quiero cambiar todo a mayuscula";
    char str2[] = " QUIERO CAMBIAR TODO A MINUSCULA";
    string_to_may(str1);
    printf("Convertido a mayúsculas: %s\n", str1);
   
    string_to_min(str2);
    printf("Convertido a minúsculas: %s\n", str2);

    return 0;
}
