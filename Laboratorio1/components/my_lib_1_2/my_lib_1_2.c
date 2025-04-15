#include "my_lib_1_2.h"

void  init_lab(void);


void  init_lab() {
    char nombre1[] ="Fernando Fabian Larrica Gastan";
    char nombre2[] = "Diego Gaston Massaferro Molina";
    char nombre3[] = "Marcos Giombini";

    printf("Laboratorio lenguaje C de %s, %s y %s.\n", nombre3,nombre2,nombre1);
}

root_t eq_solver(coeff_t *coeficientes){
    root_t resultado;
    double a = coeficientes->a;
    double b = coeficientes->b;
    double c = coeficientes->c;

    double determinante = b * b - 4 * a * c;

    /***********************************************************
    determinante mayor a 0 es "normal"
    ************************************************************/

    if (determinante > 0){
        resultado.R1 = (-b + sqrt(determinante)) / (2 * a);
        resultado.R2 = (-b - sqrt(determinante)) / (2 * a);
        printf("Las soluciones son: x1 = %.2f y x2 = %.2f\n", resultado.R1, resultado.R2);
    }

    /***********************************************************
    determinante igual a 0 tiene raices dobles
    ************************************************************/

    else if (determinante == 0) {
        resultado.R1 = -b / (2 * a);
        printf("La solución es una raíz doble: x = %.2f\n", resultado.R1);
    } 

    /***********************************************************
    determinante menor a 0 tiene raices complejas
    ************************************************************/

    else {
        resultado.R1 = -b / (2 * a);
        resultado.R2 = sqrt(-determinante) / (2 * a); // tomo R2 como parte imaginaria para no romper la escructura
        printf("Las soluciones son complejas:\n");
        printf("x1 = %.2f + %.2fi\n", resultado.R1, resultado.R2);
        printf("x2 = %.2f - %.2fi\n", resultado.R1, resultado.R2);
    }
    return resultado;
}

int32_t bin2dec(int32_t binary, bool sign){
    int32_t decimal = 0;
    int contador = 0;
    int exponente = 0;
    int32_t original = binary;

    while (binary > 0) {
        int digito = binary % 10;
        if (digito != 0 && digito != 1) {
            printf("No ha ingresado un numero binario valido. \n");
            return 0;
        }
        if ((binary & 1) == 1){

            exponente = pow(2,contador);
            contador++;
            decimal += exponente;
            binary = binary >> 1;
        }
        else{
            binary = binary >> 1;
            contador++;
        }
    }
    if (sign){
        if ((original >> (contador - 1)) & 1) {
            exponente = pow(2,contador);
            decimal -= exponente;
            printf("El numero binario pasado a decimal es : - %d \n", decimal);
        }
    }
    printf("El numero binario pasado a decimal es : %d \n", decimal);
    return ;
}

void print_reverse_array(void *array, size_t data_type, size_t array_size){
    uint8_t *inversoAlgo = (uint8_t *)array;

    printf("Array en orden inverso:\n");

    for (int i = array_size - 1; i >= 0; i--) {
        void *element = inversoAlgo + i * data_type;

        if (data_type == sizeof(int)) {
            printf("%d ", *(int *)element);
        } else if (data_type == sizeof(float)) {
            printf("%.2f ", *(float *)element);
        } else if (data_type == sizeof(double)) {
            printf("%.2lf ", *(double *)element);
        } else if (data_type == sizeof(char)) {
            printf("%c ", *(char *)element);
        } else {
            printf("Tipo desconocido ");
        }
    }

    printf("\n");
    return;
}

void max_index(void *array, size_t data_type, size_t array_size){
    uint8_t *ptr = (uint8_t *)array;
    size_t max_idx = 0;

    if (array_size == 0) return;

    for (size_t i = 1; i < array_size; i++) {
        void *current = ptr + i * data_type;
        void *max = ptr + max_idx * data_type;

        if (data_type == sizeof(int)) {
            if (*(int *)current > *(int *)max)
                max_idx = i;
        } else if (data_type == sizeof(float)) {
            if (*(float *)current > *(float *)max)
                max_idx = i;
        } else if (data_type == sizeof(double)) {
            if (*(double *)current > *(double *)max)
                max_idx = i;
        } else if (data_type == sizeof(char)) {
            if (*(char *)current > *(char *)max)
                max_idx = i;
        }
    }

    printf("Máximo en índice: %zu\n", max_idx);
return;
}
void min_index(void *array, size_t data_type, size_t array_size){
    uint8_t *ptr = (uint8_t *)array;
    size_t min_idx = 0;

    if (array_size == 0) return;

    for (size_t i = 1; i < array_size; i++) {
        void *current = ptr + i * data_type;
        void * min = ptr +  min_idx * data_type;

        if (data_type == sizeof(int)) {
            if (*(int *)current < *(int *) min)
                min_idx = i;
        } else if (data_type == sizeof(float)) {
            if (*(float *)current < *(float *) min)
                min_idx = i;
        } else if (data_type == sizeof(double)) {
            if (*(double *)current < *(double *) min)
                min_idx = i;
        } else if (data_type == sizeof(char)) {
            if (*(char *)current < *(char *) min)
                min_idx = i;
        }
    }

    printf("Mínimo en índice: %zu\n",  min_idx);
return;
}

matriz_t matrix_sub(matriz_t A, matriz_t B){
    matriz_t resultado;
    
    // Validar que tengan las mismas dimensiones
    if (A.filas != B.filas || A.columnas != B.columnas) {
        printf("Error: Las matrices no tienen las mismas dimensiones\n");
        resultado.datos = NULL;
        resultado.filas = 0;
        resultado.columnas = 0;
        return resultado;
    }
    
    resultado = crear_matriz(A.filas, A.columnas);
    
    for (size_t i = 0; i < A.filas; i++) {
        for (size_t j = 0; j < A.columnas; j++) {
            resultado.datos[i][j] = A.datos[i][j] - B.datos[i][j];
        }
    }
    
return resultado;
}

matriz_t crear_matriz(size_t filas, size_t columnas) {
    matriz_t matriz;
    matriz.filas = filas;
    matriz.columnas = columnas;

    matriz.datos = (int32_t **)malloc(filas * sizeof(int32_t *));
    for (size_t i = 0; i < filas; i++) {
        matriz.datos[i] = (int32_t *)malloc(columnas * sizeof(int32_t));
    }

    return matriz;
}

int consonantes(char *puntero){
    int consonante = 0;
    while (*puntero != '\0')
    {
        
        if ((*puntero > 64 && *puntero < 91) || (*puntero > 96 && *puntero < 123))
        //if ((*puntero >= 'A' && *puntero <= 'Z') || (*puntero >= 'a' && *puntero <= 'z'))
        {
            switch (*puntero)
            {
            case 'A':
                puntero++;
                break;
            case 'a':
                puntero++;
                break;
            case 'E':
                puntero++;
                break;
            case 'e':
                puntero++;
                break;
            case 'I':
                puntero++;
                break;
            case 'i':
                puntero++;
                break;
            case 'O':
                puntero++;
                break;
            case 'o':
                puntero++;
                break;
            case 'U':
                puntero++;
                break;
            case 'u':
                puntero++;
                break;   
            default:
                consonante++;
                break;
            }
        }
        puntero++;     
    }
    printf("La cantidad de consonantes es de %d. \n",consonante);
    return consonante;

}
int vocales(char *puntero){
    int Vocal = 0;
    while (*puntero != '\0')
    {
        
        if ((*puntero > 64 && *puntero < 91) || (*puntero > 96 && *puntero < 123))
        //if ((*puntero >= 'A' && *puntero <= 'Z') || (*puntero >= 'a' && *puntero <= 'z'))
        {
            switch (*puntero)
            {
            case 'A':
                Vocal++;
                break;
            case 'a':
                Vocal++;
                break;
            case 'E':
                Vocal++;
                break;
            case 'e':
                Vocal++;
                break;
            case 'I':
                Vocal++;
                break;
            case 'i':
                Vocal++;
                break;
            case 'O':
                Vocal++;
                break;
            case 'o':
                Vocal++;
                break;
            case 'U':
                Vocal++;
                break;
            case 'u':
                Vocal++;
                break;   
            default:
                puntero++;
                break;
            }
        }
        puntero++;     
    }
    
    printf("La cantidad de vocales es de %d. \n",Vocal);
    return Vocal;
}

char reverse_string(char *puntero){
    int largo = strlen(puntero);
    printf("El largo de la cadena de texto es: %d. \n", largo);

    char aux;
    int inicio = 0;
    int final = largo-1; // agregar -1 xq sino el text comienza con \n y no lo muestra 

    while (inicio < final){
        aux = puntero[inicio];
        puntero[inicio] = puntero[final];
        puntero[final] = aux;

        inicio++;
        final--;
    }
    return puntero;
}
