#ifndef MY_LIB_2_H_
#define MY_LIB_2_H_

#include <stdlib.h>
#include <string.h>

void my_lib_2_function();

#define FORMAT_CI 8
#define FORMAT_NAME 15

typedef struct Estudiante_s {
    char nombre[FORMAT_NAME];
    char apellido[FORMAT_NAME];
    char CI[FORMAT_CI];
    int grado;
    int promCalif;
    struct Estudiante_s *siguiente; // VER SI DEEB SER PUNTERO O NO
}Estudiante_t;

typedef struct Sort_s {
    Estudiante_t *actualPtr;
    int sortCounter;
}Sort_t;

// Funciones de menu
void accesoMenu(); //OK
void addNewStudentMenu(); // OK
void displayListMenu(); //OK
void deleteStudentMenu(); //OK

//Agregar nuevo estudiante

// Funciones agregar estudiante
Estudiante_t *inicializarEstudiante(); // Agrega nuevo elemento a la lista apuntando a NULL. //OK
Estudiante_t *cargarDatosMenu(Estudiante_t *studentPtr); // Carga datos de estudiante en Estudiante inicializado. //OK
Estudiante_t *buscaUltimoLista(); // Busca el ultimo estudiante de la lista iterando hasta encontrar NULL.

// Funciones display de lista
Estudiante_t *inicializarLista(); // Agrega el primer elemento de la lista apuntando a NULL y con los campos vacios.
Sort_t sortStudentsNames(Estudiante_t *actualPtr, int sortCounter);
Sort_t sortStudentsLastNames(Estudiante_t *actualPtr, int sortCounter);
Sort_t sortStudentsCI(Estudiante_t *actualPtr, int sortCounter);
void sortListByNames(Estudiante_t *listPtr);
void sortListByLastNames(Estudiante_t *listPtr);
void sortListByCI(Estudiante_t *listPtr);
void makeListCircular(Estudiante_t *listPtr);
Estudiante_t  *pointFirstName(Estudiante_t *listPtr);
Estudiante_t  *pointFirstLastName(Estudiante_t *listPtr);
Estudiante_t  *pointFirstCI(Estudiante_t *listPtr);

// Print List
void printListHeader(); // Auxiliar
void printStudentRow(Estudiante_t *actualPtr); // Auxiliar
void displayList(Estudiante_t *listPtr); // Funcion prinicpal

//Borrar elemento de lista
void deleteStudent(Estudiante_t *actualPtr, Estudiante_t *listPtr);
void deleteStudentOptions(Estudiante_t *actualPtr, Estudiante_t *listPtr);
void deleteStudentMenu(Estudiante_t *listPtr);

#endif
