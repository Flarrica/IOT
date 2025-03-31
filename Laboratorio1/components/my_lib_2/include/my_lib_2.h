#ifndef MY_LIB_2_H_
#define MY_LIB_2_H_
#define FORMAT_CI 8
#define FORMAT_NAME 15

typedef struct Estudiante_s {
    char nombre[FORMAT_NAME];
    char apellido[FORMAT_NAME];
    char CI[FORMAT_CI];
    int grado;
    int promCalif;
    Estudiante_t siguiente; // VER SI DEEB SER PUNTERO O NO
}Estudiante_t;

typedef struct Sort_s {
    Estudiante_t *actualPtr;
    int sortCounter;
}Sort_t;

// Funciones de menu
void accesoMenu();
void addNewStudentMenu();
void displayListMenu();
void findStudentMenu();

// Funciones de lista
Estudiante_t *inicializarLista(); // Agrega el primer elemento de la lista apuntando a NULL y con los campos vacios.
Estudiante_t *inicializarEstudiante(); // Agrega nuevo elemento a la lista apuntando a NULL.
Estudiante_t *buscaUltimoLista(); // Busca el ultimo estudiante de la lista iterando hasta encontrar NULL.
Estudiante_t *enganchaLista(); // Coloca el nuevo estudiante al final de la lista manejando punteros.


void my_lib_2_function();

#endif
