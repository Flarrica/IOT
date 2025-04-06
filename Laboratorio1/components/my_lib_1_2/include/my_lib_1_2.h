#ifndef MY_LIB_1_2_H_
#define MY_LIB_1_2_H_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void my_lib_1_2_function();
root_t eq_solver(coeff_t *coeficientes);

typedef struct {
    double a;
    double b;
    double c;
} coeff_t;

typedef struct {
    double R1;
    double R2;
} root_t;




#endif
