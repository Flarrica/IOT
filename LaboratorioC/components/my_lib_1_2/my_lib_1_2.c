#include "my_lib_1_2.h"


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

int32_t bin2dec(int32_t binary, bool sign) {
    int32_t decimal = 0;
    int contador = 0;
    int32_t original = binary;

    // Validación y conversión
    while (binary > 0) {
        int digito = binary % 10;
        if (digito != 0 && digito != 1) {
            printf("No ha ingresado un numero binario valido.\n");
            return 0;
        }
        decimal += digito * pow(2, contador);
        binary /= 10;
        contador++;
    }

    // Si tiene signo y el bit más significativo es 1
    if (sign && ((original / (int)pow(10, contador - 1)) % 10 == 1)) {
        int32_t exponente = pow(2, contador);
        decimal -= exponente;
        printf("El numero binario pasado a decimal es: -%d\n", decimal);
    } else {
        printf("El numero binario pasado a decimal es: %d\n", decimal);
    }

    return decimal;
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
        
        if ((*puntero > 64 && *puntero < 91) || (*puntero > 96 && *puntero < 123)){
            if (*puntero != 'A' && *puntero != 'a' &&
                *puntero != 'E' && *puntero != 'e' &&
                *puntero != 'I' && *puntero != 'i' &&
                *puntero != 'O' && *puntero != 'o' &&
                *puntero != 'U' && *puntero != 'u') {
                consonante++;
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
        
        if (*puntero == 'A' || *puntero == 'a' ||
            *puntero == 'E' || *puntero == 'e' ||
            *puntero == 'I' || *puntero == 'i' ||
            *puntero == 'O' || *puntero == 'o' ||
            *puntero == 'U' || *puntero == 'u') {
            Vocal++;
        }
        puntero++;     
    }
    
    printf("La cantidad de vocales es de %d. \n",Vocal);
    return Vocal;
}

char reverse_string(char *puntero){
    int largo = strlen(puntero);

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
    return 0;
}

int swap(void *elem_1, void *elem_2, size_t data_type) {
    if (elem_1 == NULL || elem_2 == NULL || data_type == 0) {
        return -1;  // Error
    }

    void *aux = malloc(data_type);
    if (aux == NULL) {
        return -1;  // Error
    }

    memcpy(aux, elem_1, data_type);      
    memcpy(elem_1, elem_2, data_type);   
    memcpy(elem_2, aux, data_type);      

    free(aux);
    return 0;  // Éxito
}

int32_t string_length(char *string) {
    if (string == NULL) {
        return -1;
    }

    int32_t length = 0;
    while (string[length] != '\0') {
        length++;
    }

    return length;
}