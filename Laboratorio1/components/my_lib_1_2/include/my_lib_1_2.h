#ifndef MY_LIB_1_2_H_
#define MY_LIB_1_2_H_

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    double R1;
    double R2;
} root_t;

typedef struct {
    double a;
    double b;
    double c;
} coeff_t;

typedef struct {
    size_t filas;
    size_t columnas;
    int32_t **datos;
} matriz_t;

void init_lab();
root_t eq_solver(coeff_t *coeficientes);
int32_t bin2dec(int32_t binary, bool sign);
void print_reverse_array(void *array, size_t data_type, size_t array_size);
void max_index(void *array, size_t data_type, size_t array_size);
void min_index(void *array, size_t data_type, size_t array_size);
matriz_t matrix_sub(matriz_t A, matriz_t B);
matriz_t crear_matriz(size_t filas, size_t columnas);
int consonantes(char *puntero);
int vocales(char *puntero);
char reverse_string(char *puntero);
int swap(void *elem_1, void *elem_2, size_t data_type);
int32_t string_length(char *string);



#endif
