#include <stdio.h>
#include <stdint.h> 
#include <string.h>

typedef struct Resultado{
    int cantLetras; // Cuenta cantidad de letras totales halladas en string.
    int cantPalabras; // Cuenta palabras
    int cantVoc; // Cuenta cantidad de vocales halladas en string.
    int cantCons; // Cuenta las consonantes.
}Resultado;


// PARTE A
void intercElem(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// PARTE B
Resultado cuentaNumVoc(char *ptr){
    // Inicializamos el resultado
    Resultado result = {0, 0, 0, 0};
    int hayPalabra = 0; // Flag de presencia de palabra
    while (*ptr != '\0')
    {
        //printf("Entra while. Letra: %d\n",result.cantLetras);
        switch (*ptr)
        {
        case 'a':
            result.cantVoc++;
            result.cantLetras++;
            hayPalabra = 1;
            break;
        case 'e':
            result.cantVoc++;
            result.cantLetras++;
            hayPalabra = 1;
            break;
        case 'i':
            result.cantVoc++;
            result.cantLetras++;
            hayPalabra = 1;
            break;
        case 'o':
            result.cantVoc++;
            result.cantLetras++;
            hayPalabra = 1;
            break;
        case 'u':
            result.cantVoc++;
            result.cantLetras++;
            hayPalabra = 1;
            break;    
        default:
            if ((*ptr >= 65 && *ptr <= 90) || (*ptr >= 97 && *ptr <= 122 )) // Compara y da TRUE si el valor ASCII esta dentro de las mayusculas o minusculas
            {
                result.cantCons++;
                result.cantLetras++;
                hayPalabra = 1;
            }else if (hayPalabra == 1)
            {
                result.cantPalabras++;
                hayPalabra = 0;
            }
            
            break;
        }
        ptr++; //Incremento puntero
    }
    return result; 
}

int main() {
    int c = 5, d = 10;
    Resultado frase;
    char str1[] = "buenas tardes amigos";
    printf("Ejercicio 3 - Parte a\n");
    printf("Antes del intercambio: c = %d, d = %d\n", c, d);
    intercElem(&c, &d);
    printf("Luego del intercambio: c = %d, d = %d\n", c, d);
    printf("Ejercicio 3 - Parte b\n");
    frase = cuentaNumVoc(str1);
    printf("Cantidad de vocales: %d\n", frase.cantVoc);
    printf("Cantidad de consonantes: %d\n", frase.cantCons);
    printf("Cantidad de palabras: %d\n", frase.cantPalabras);
    printf("Cantidad de letras: %d\n", frase.cantLetras);

    return 0;
}
