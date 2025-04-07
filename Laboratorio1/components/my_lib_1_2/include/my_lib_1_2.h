#ifndef MY_LIB_1_2_H_
#define MY_LIB_1_2_H_

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

void my_lib_1_2_function();
root_t eq_solver(coeff_t *coeficientes);
int32_t bin2dec(int32_t binary, bool sign);
void print_reverse_array(void *array, size_t data_type, size_t array_size);
void max_index(void *array, size_t data_type, size_t array_size);
void min_index(void *array, size_t data_type, size_t array_size);



typedef struct {
    double R1;
    double R2;
} root_t;

typedef struct {
    double a;
    double b;
    double c;
} coeff_t;


#endif
