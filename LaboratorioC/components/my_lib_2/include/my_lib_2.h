#ifndef MY_LIB_2_H_
#define MY_LIB_2_H_

#include <stdlib.h>
#include <string.h>


// Constantes usadas para display de listas y verificacion de errores
#define FORMAT_NAME 15
#define FORMAT_LASTNAME 15
#define FORMAT_CI 9
#define FORMAT_GRADE 5
#define FORMAT_PROMCALIF 12

// Códigos de error
typedef enum {
    NO_ERROR,
    ERROR_NOMBRE_LARGO,
    ERROR_APELLIDO_LARGO,
    ERROR_CI_LARGO,
    ERROR_GRADO_RANGO,
    ERROR_CALIFICACIONES_RANGO
} CodigoError_t;

typedef enum {
    PRIMERO = 1,
    SEGUNDO,
    TERCERO,
    CUARTO,
    QUINTO,
    SEXTO
} Grado_t;

typedef enum {
    SIN_FILTRO,
    NOMBRE,
    APELLIDO,
    CI
} SortBy_t;

typedef struct Estudiante_s {
    char nombre[FORMAT_NAME];
    char apellido[FORMAT_LASTNAME];
    char CI[FORMAT_CI];
    Grado_t grado;
    int  promCalif;
    struct Estudiante_s *siguiente;
}Estudiante_t;

typedef struct Sort_s {
    struct Estudiante_s *actualPtr;
    int sortCounter;
}Sort_t;

// Funcion de validacion de datos
CodigoError_t validacionNuevoEstudiante (char nombre[FORMAT_NAME], char apellido[FORMAT_LASTNAME], char CI[FORMAT_CI], Grado_t grado, int  promCalif);
void printErrorNuevoEstudiante(CodigoError_t error);
CodigoError_t validacionFiltroLista (char nombre[FORMAT_NAME], char apellido[FORMAT_LASTNAME], char CI[FORMAT_CI], Grado_t grado, int  promCalif);
void printErrorFiltroLista(CodigoError_t error);



//FUNCIONES AUXILIARES -  Agregar estudiante
Estudiante_t *ultimoElementoLista(Estudiante_t *listPtr); // Busca el ultimo estudiante de la lista iterando hasta encontrar NULL.

//FUNCIONES AUXILIARES -  Display de lista
Estudiante_t * buscaMenorElemento(Estudiante_t *listPtr, SortBy_t filtro);
Estudiante_t * armarListaOrdenada(Estudiante_t *listPtr, SortBy_t filtro);
void printListHeader();
void printStudentRow(Estudiante_t *actualPtr);
char *traduceNota(int promCalif);

//FUNCIONES AUXILIARES - Borrar elemento de la lista
void deleteStudentElement(Estudiante_t *actualPtr, Estudiante_t *listPtr);
void deleteStudentOptions(Estudiante_t *actualPtr, Estudiante_t *listPtr);


// Funciones principales
Estudiante_t * agregarNuevoEstudiante(Estudiante_t *listPtr, char nombre[FORMAT_NAME], char apellido[FORMAT_LASTNAME], char CI[FORMAT_CI], Grado_t grado, int  promCalif); // Agrega nuevo elemento a la lista apuntando a NULL. //OK
Estudiante_t * printfList(Estudiante_t *listPtr, SortBy_t filtro);
void deleteStudentFromList(Estudiante_t *listPtr, char CI[FORMAT_CI]); //OK
#endif