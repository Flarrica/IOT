
#include <stdio.h>

void string_to_caps(char *string) {
    // Se Recorrer la cadena hasta encontrar el carácter nulo '\0'
    while (*string != '\0') {
        // Si el carácter es una letra minúscula (entre 'a' y 'z')
        if (*string >= 'a' && *string <= 'z') {
            *string = *string - 'a' + 'A';  // Convertir a mayúscula
        }
        string++;  // Avanzar al siguiente carácter
    }
}

void string_to_min(char *string) 
{
    // Recorrer la cadena hasta encontrar el carácter nulo '\0'
    while (*string != '\0') {
        // Si el carácter es una letra mayúscula (entre 'A' y 'Z')
        if (*string >= 'A' && *string <= 'Z') {
            *string = *string - 'A' + 'a';  // Convertir a minúscula
        }
        string++;  // Avanzar al siguiente carácter
    }
}

void solicitar_texto() {
    char input[100];
    int choice;

    // Se le pide al usuario que ingrese una cadena a la cual luego se pasara todo a mayuscila o minuscula segun la opcion del usuario
    printf("Ingrese una oración: ");
    fgets(input, sizeof(input), stdin);
    
    // Se imprime un menu para que el usario elija que quiere hacer convertir a mayúsculas o minúsculas
    printf("Seleccione una opción:\n");
    printf("1. Convertir a mayúsculas\n");
    printf("2. Convertir a minúsculas\n");
    printf("Ingrese 1 o 2: ");
    scanf("%d", &choice);

    getchar();  

    // Segun la opcion que elugio el usuario se llama a la función de eleccion
    if (choice == 1) {
        string_to_caps(input);  // Se llama a la funcion que converte a mayúsculas
        printf("La oración en mayúsculas es: %s\n", input);
    } else if (choice == 2) {
        string_to_min(input);  // Se lama a la funcion que convierte a minúsculas
        printf("La oración en minúsculas es: %s\n", input);
    } else {
        printf("Opción no válida.\n");
    }
}

int main() {
    solicitar_texto();  // Se llamar a la función que gestiona todo el proceso
    
    return 0;
}
