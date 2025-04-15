#include <stdio.h>
#include <stdbool.h>
#include "Global.h"


/* 
Para compilar, ingresar los siguientes comandos en "cmd"

(Ir al directorio)
cd Desktop\IoT\IOT\Laboratorio1

(Borrar build anterior)
rmdir /s /q build
del CMakeCache.txt

(Compilar)
build.cmd compile

(Correr)
build.cmd run

*/
// VERIFICAR SI ESTA BIEN DECLARADA EN EL MAIN O DEBE IR EN LA LIBRERIA
// *listStudent; // Apunta al primer elemento de la lista. Una vez que se crea el primer elemento, no se va a mover de alli.
//Estudiante_t *listResult; // Se crea lista vacia para almacenar lso resultados de busquedas.
int main() {






    // 1. init_lab
    init_lab();

    // 2. eq_solver
    coeff_t coef;
    coef.a = 1;
    coef.b = -3;
    coef.c = 2;
    eq_solver(&coef); // Debería dar x1 = 2.00 y x2 = 1.00

    // 3. bin2dec
    printf("\nPrueba de bin2dec:\n");
    bin2dec(1101, false); // 1101 = 13
    bin2dec(11111111111111111111111111111111, true); // Prueba con signo

    // 4. print_reverse_array
    printf("\nPrueba de print_reverse_array:\n");

    int enteros[] = {10, 20, 30, 40, 50};
    print_reverse_array(enteros, sizeof(int), 5);

    float flotantes[] = {1.5, 2.5, 3.5};
    print_reverse_array(flotantes, sizeof(float), 3);

    char letras[] = {'A', 'B', 'C', 'D'};
    print_reverse_array(letras, sizeof(char), 4);

    // 5. max_index y min_index
    printf("\nPrueba de max_index y min_index:\n");

    max_index(enteros, sizeof(int), 5); // Debería dar índice 4
    min_index(enteros, sizeof(int), 5); // Debería dar índice 0

    max_index(flotantes, sizeof(float), 3); // Índice 2
    min_index(flotantes, sizeof(float), 3); // Índice 0

    max_index(letras, sizeof(char), 4); // Índice 3 (D)
    min_index(letras, sizeof(char), 4); // Índice 0 (A)

    //listStudent = inicializarEstudiante(listStudent); // INICIALIZAR LISTA DE ESTUDIANTES
    //listResult = inicializarEstudiante(listResult); // INICIALIZAR LISTA DE RESULTADOS DE BUSQUEDA
    //accesoMenu(listStudent, listResult); // Entramos al MENU

    //my_lib_1_1_function();
   //my_lib_1_2_function();
    //my_lib_2_function();



    return 0;
}
