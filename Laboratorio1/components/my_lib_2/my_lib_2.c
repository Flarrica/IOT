#include <stdio.h>
#include "my_lib_2.h"

int opcionElegida;
char bufferName[FORMAT_NAME];
char bufferLastName[FORMAT_NAME];
char bufferCI[FORMAT_CI];

// Declaramos variables
Estudiante_t *newStudent; // La usamos para crear nuevo elemento en la lista de estidiantes. Puntero "Siguiente" apunta a NULL.
//Estudiante_t *listFirst; Esta declarada en el main. Confirmar en donde debe declararse.
Estudiante_t *listBuffer; // Puntero auxiliar para iterar en la lista.

// INICIALIZAR VARIABLES
Estudiante_t *inicializarEstudiante() { 
    Estudiante_t* auxPtr = malloc(sizeof(Estudiante_t)); //Reservamos memoria para un estudiante
    // Inicializar campos con strcpy (para cadenas)
    strcpy(auxPtr->nombre, ""); // Inicializamos vacio
    strcpy(auxPtr->apellido, ""); // Inicializamos vacio
    strcpy(auxPtr->CI, ""); // Inicializamos vacio
    // Inicializar campos enteros directamente
    auxPtr->grado = 0; // Inicializamos 0
    auxPtr->promCalif = 0; // Inicializamos 0
    // Inicializar puntero siguiente a NULL. Cerrar la lista
    auxPtr->siguiente = NULL;
    return auxPtr;
}
// CARGAR DATOS NUEVO ESTUDIANTE
Estudiante_t *cargarDatos(Estudiante_t studentPtr) { // Ver tipo de variable
    Estudiante_t auxPTr = studentPtr;
    printf("Ingrese el nombre del estudiante: ");
    scanf("%s", auxPTr.nombre);

    printf("Ingrese el apellido del estudiante: ");
    scanf("%s", auxPTr.apellido);

    printf("Ingrese el CI del estudiante: ");
    scanf("%s", auxPTr.CI);

    printf("Ingrese el grado del estudiante: ");
    scanf("%d", &auxPTr.grado);

    printf("Ingrese el promedio de calificaciones del estudiante: ");
    scanf("%d", &auxPTr.promCalif);

    // Imprimir los datos del estudiante (opcional)
    printf("\n----------------------------------------\n");
    printf("Datos del estudiante:\n");
    printf("Nombre: %s\n", auxPTr.nombre);
    printf("Apellido: %s\n", auxPTr.apellido);
    printf("CI: %s\n", auxPTr.CI);
    printf("Grado: %d\n", auxPTr.grado);
    printf("Promedio: %d\n", auxPTr.promCalif);
    printf("----------------------------------------\n");
}
// BUSCAR ULTIMO EN LISTA
Estudiante_t *buscaUltimoLista(Estudiante_t *listPtr) {// Se le da como entrada el puntero al inicio de la lista
    Estudiante_t *auxPtr = listPtr; //Declaro e inicializo un puntero auxiliar que apunta al inicio de la lista.
    while (auxPtr->siguiente != NULL) { // Esta condicion se da cuando se llega al elemento final de la lista.
        auxPtr = auxPtr->siguiente; // Se itera apuntando el puntero auxiliar a la siguiente posicion
    }
    return auxPtr; //Devolvemos el puntero para que lo tome otra funcion
}
// COMPARAR STRINGS
Sort_t sortStudentsNames(Estudiante_t *actualPtr, int sortCounter) { // Odrnea alfabeticamente y devuelve el siguiente elemento
    Estudiante_t *nextPtr = actualPtr->siguiente; // Apunto a los dos primero elementos para comparar
    int ordenName = strcmp(actualPtr->nombre,nextPtr->nombre); // str1 > str2 es positivo, str1 = str2 es 0, str1 < str2 es negativo
    if (ordenName > 0) { // Permutar lugares
        actualPtr->siguiente = nextPtr->siguiente;
        nextPtr->siguiente = actualPtr;
        actualPtr = nextPtr;// Pasar a siguiente estudiante
        sortCounter++;
    } else if (ordenName < 0) { 
        actualPtr = nextPtr;// No hacer nada. Pasar a siguiente estudiante
    } else { // Sortear por cedula ascendente
        int ordenCI = strcmp(actualPtr->CI,nextPtr->CI); // str1 > str2 es positivo, str1 = str2 es 0, str1 < str2 es negativo
        if (ordenCI > 0) { // Permutar lugares
            actualPtr->siguiente = nextPtr->siguiente;
            nextPtr->siguiente = actualPtr;
            actualPtr = nextPtr;// Pasar a siguiente estudiante
            sortCounter++;
        } else {
            actualPtr = nextPtr; // No hacer nada. Pasar a siguiente estudiante
        }
    }
    Sort_t sortResult = {actualPtr, sortCounter};
    return sortResult;
}
// SORTEAR LISTA POR NOMBRE
void sortListByNames(Estudiante_t *listPtr) { // Odrnea alfabeticamente y devuelve el siguiente elemento
    
    Estudiante_t *actualPtr = listPtr; // Apunto a los dos primeros elementos para comparar
    Estudiante_t *nextPtr = actualPtr->siguiente;
    int sortCounter = 0;
    Sort_t sortResult = {actualPtr, sortCounter};
    do
    {
        sortResult.sortCounter = 0;
        while (nextPtr != NULL) {
            sortResult = sortStudentsNames(sortResult.actualPtr, sortResult.sortCounter);
        }
    } while (sortResult.sortCounter != 0);
    
    return 0;
}// Estoy por aca <---------------------------------------------------------------------------------------------------------------------------
// INICIO FUNCIONES DE MENU/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void accesoMenu(Estudiante_t *listPtr) {
    int flagMenu = 0;
    do {
        //Muestra las opciones del Menu
        printf("\n---------Sistema de gestion de alumnos de IoT---------\n");
        printf("1. Ingresar nuevo alumno/a\n");
        printf("2. Consultar listado de alumnos/a\n");
        printf("3. Buscar alumno/a\n");
        printf("4. Salir\n");
        scanf("%d", &opcionElegida);
        // Hasta aca entra y toma una opcion del menu
        
        switch (opcionElegida) {
            case '1':
                addNewStudentMenu(listPtr);
                break;
            case '2':
                displayListMenu();
                break;
            case '3':
                findStudentMenu();
                break;
            case '4':
                printf("Saliendo...");
                break;
            default:
                printd("Opcion invalida. Ingrese nuevamente.\n");
        }
    }
    while (opcionElegida != 4);
    return 0;
}

void addNewStudentMenu(Estudiante_t *listPtr) { // Toma como entrada el puntero al primer elemento de la lista.
    do
    {   
        printf("Desea agregar un nuevo estudiante?");
        printf("1. Si. -> Agregar datos");
        printf("2. No. -> Volver a menu principal");
        scanf("%d", &opcionElegida);
        switch (opcionElegida) {
            case '1':
                Estudiante_t* newStudent = inicializarEstudiante();// Inicializamos el nuevo elemento.
                newStudent = cargarDatos(*newStudent);// Cargamos los datos del estudiante.
                Estudiante_t* ultimoList = buscaUltimoLista(listPtr);// Apuntamos al ultimo lugar de la lista.
                ultimoList->siguiente = newStudent;// "Enganchamos" nuevo elemento a la lista. newStudent->siguiente ya es NULL por inicializacion
                break;
            case '2':
                printf("Saliendo...");
                break;
            default:
                printd("Opcion invalida. Ingrese nuevamente.\n");
        }
    } while (opcionElegida != 2);
    return 0;
}

void displayListMenu() { // Toma como entrada el puntero al primer elemento de la lista.
    do
    {   
        printf("Con que orden desea visualizar la lista?");
        printf("1. Por nombre. Ascendente");
        printf("2. Por apellido. Ascendente");
        printf("3. CI. Ascendente");
        printf("4. Salir");
        scanf("%d", &opcionElegida);
        switch (opcionElegida) {
            case '1':
                /* codigo */
                break;
            case '2':
                /* codigo */
                break;
            case '3':
                /* codigo */
                break;
            case '4':
                /* codigo */
                break;
            default:
                printd("Opcion invalida. Ingrese nuevamente.\n");
        }
    } while (opcionElegida != 2);
    return 0;
}

void findStudentMenu() {
    do {
        //Muestra las opciones del Menu
        printf("\n---------Sistema de gestion de alumnos IoT---------\n");
        printf("1. Ingresar nuevo alumno/a\n");
        printf("2. Consultar listado de alumnos/a\n");
        printf("3. Buscar alumno/a\n");
        printf("4. Salir\n");
        printf("-----------------------------------------------------\n");
        printf("Indique un numero con la opcion que desee ejecutar: ");
        scanf("%d", &opcionElegida);
        // Hasta aca entra y toma una opcion del menu
        
        switch (opcionElegida) {
            case '1':
                addNewStudent();
                break;
            case '2':
                displayList();
                break;
            case '3':
                findStudent();
                break;
            case '4':
                printf("Saliendo...");
                break;
            default:
                printd("Opcion invalida. Ingrese nuevamente.\n");
        }
    }
    while (opcionElegida != 4);
    return 0;
}
// FIN FUNCIONES  DE MENU///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void my_lib_2_function() {
    printf("Funcion de my_lib_2 ejecutada.\n");
}
