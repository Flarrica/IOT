#include <stdio.h>

// Creamos una función que lo que hace es verificar si el texto ingresado no contiene nnumeros 
int contieneNumero(char *texto) {
    while (*texto) {
        if (*texto >= '0' && *texto <= '9') {
            return 1;  // Si se encuentra un número entre 0 y 9, retornamos 1 (verdadero)
        }
        texto++;
    }
    return 0;  // Si no encontra números, retornamos 0 (falso)
}

// Esta función cuenta las palabras en el texto
void string_words(char *string) {
    char texto[50];
    char *string = texto; 
    
    while (1) {
        printf("\nIngrese un texto que no supere los 50 caracteres: \n\n");
        fgets(texto, sizeof(texto), stdin); // Lee el texto ingresado por el usuario. se usa "fgets para que como espacios"
        
        // Se hace una verificacion si el texto ingresado es vacio
        if (*texto == '\0') { 
            printf("Número de palabras: 0\n"); 
            return; 
        }

        // Validar si el texto contiene números
        if (contieneNumero(texto)) {
            printf("Error: El texto no debe contener números. Intenta de nuevo.\n");
            continue;  // Si se detecta número, pide el texto nuevamente que eslo que hace el contnue vuelve
                        // a comnzar el siclo while
        }

        int count = 0;
        int inPalabra = 0;

        // las palabras se cuentas verificando si hay espacios saltos de line o tabulacion
        while (*string) 
        {
            if (*string == ' ' || *string == '\t' || *string == '\n') {
                inPalabra = 0;
            } else if (!inPalabra) {
                inPalabra = 1;
                count++;
            }
            string++;
        }

        printf("\n--------------------------------------------------------\n");
        printf("Texto: \"%s\"\n", texto);
        printf("Número de palabras: %d\n", count);
        printf("\n--------------------------------------------------------\n");

        break;  // Si todo está bien, salimos del ciclo
    }
}

int main() {
    char texto[50];
    string_words(texto);
    return 0;
}
