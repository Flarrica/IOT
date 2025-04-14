#include <stdio.h>
#include "my_lib_1_1.h"
#include <time.h> 
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
// Funcion para pasar de min a may
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
// Funcion para pasar de may a min
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
//Solicita el texto para ejejutar las funciones anteriores
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
// Funcion para copiar un string en otro string
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

// Función que calcula la diferencia en días entre dos fechas
int days_left(date_t start, date_t finish) {
    struct tm tm_start = {0}, tm_finish = {0}; // Estructuras de tiempo inicializadas en 0

    // Cargamos los datos de la primera fecha en la estructura tm
    tm_start.tm_mday = start.dias;            // Día del mes
    tm_start.tm_mon = start.mes - 1;          // Mes (0 a 11, por eso restamos 1)
    tm_start.tm_year = start.año - 1900;      // Años desde 1900

    // Cargamos los datos de la segunda fecha
    tm_finish.tm_mday = finish.dias;
    tm_finish.tm_mon = finish.mes - 1;
    tm_finish.tm_year = finish.año - 1900;

    // Convertimos las estructuras tm a tiempo en segundos desde 1970 (epoch time)
    time_t time_start = mktime(&tm_start);
    time_t time_finish = mktime(&tm_finish);

    // Calculamos la diferencia en segundos, y luego la convertimos a días
    double difference = difftime(time_finish, time_start) / (60 * 60 * 24);

    // Devolvemos la diferencia como número entero
    return (int)difference;
}

// Esta función pide al usuario las fechas por consola, calcula la diferencia y la muestra
void ejecutarCalculoDeDias() {
    date_t fecha1, fecha2;

    // Pedimos la primera fecha al usuario
    printf("Ingrese la primera fecha (DD MM AAAA): ");
    scanf("%d %d %d", &fecha1.dias, &fecha1.mes, &fecha1.año);

    // Pedimos la segunda fecha
    printf("Ingrese la segunda fecha (DD MM AAAA): ");
    scanf("%d %d %d", &fecha2.dias, &fecha2.mes, &fecha2.año);

    // Calculamos la diferencia en días usando la función days_left
    int dias = days_left(fecha1, fecha2);

    // Mostramos el resultado al usuario en dias 
    printf("\n------------------------------------------------\n");
    printf("Diferencia en días: %d\n", dias);
    printf("------------------------------------------------\n");
}
// Funcion para encontrar una SUb cadena 
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


// Se crea una función que realiza el producto de dos números complejos
complex_t prod(complex_t a, complex_t b) {
    complex_t result;
    result.real = (a.real * b.real) - (a.imag * b.imag);
    result.imag = (a.real * b.imag) + (a.imag * b.real);
    return result;
}

// Esta función maneja toda la lógica: Pide el numero por consola, llama a la funcion que hace la operacion, imprime el resultado de la multiplicacion
void procesarMultiplicacionCompleja() {
    complex_t num1, num2, resultado;

    printf("Ingrese la parte real e imaginaria respectivamente del primer número complejo:\n ");
    scanf("%lf %lf", &num1.real,&num1.imag);

    printf("Ingrese la parte real e imagnaria respectivamente del segundo número complejo:\n ");
    scanf("%lf %lf", &num2.real, &num2.imag);


    resultado = prod(num1, num2);

    printf("\n------------------------------------------------\n");
    printf("El resultado de la multiplicación es: %.2f + %.2fi\n", resultado.real, resultado.imag);
    printf("------------------------------------------------\n");
}

complex_t sum(complex_t a, complex_t b) {
    complex_t result;
    result.real = a.real + b.real;
    result.imag = a.imag + b.imag;
    return result;
}

// Función que se encarga de todo: Se pide el valor por consola, se sumar y se muestra el resultado
void procesarSumaCompleja() {
    complex_t num1, num2, resultado;
    // Se pide por consola los valores del primer numero complejo, parte real y imaginario
    printf("\n--------------------------------------------------\n");
    printf("Ingrese la parte real del primer número complejo: ");
    scanf("%lf", &num1.real);
    printf("Ingrese la parte imaginaria del primer número complejo: ");
    scanf("%lf", &num1.imag);

    printf("\n--------------------------------------------------\n");
    
    // Se pide por consola los valores del primer numero complejo, parte real y imaginario
    printf("Ingrese la parte real del segundo número complejo: ");
    scanf("%lf", &num2.real);
    printf("Ingrese la parte imaginaria del segundo número complejo: ");
    scanf("%lf", &num2.imag);
    printf("\n--------------------------------------------------\n");

    resultado = sum(num1, num2);

    // Se muestra el valor de la suma
    printf("\n------------------------------------------------\n");
    printf("El valor de la suma compleja es: %.2f + %.2fi\n", resultado.real, resultado.imag);
    printf("------------------------------------------------\n");
}