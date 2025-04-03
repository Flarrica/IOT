#include <stdio.h>
#include "my_lib_2.h"

// Pongo funcion de ejemplo para probar funcionamiento de libreria

void my_lib_2_function() {
    printf("Funcion de la libreria _2 ejecutada con exito. Aprobado por Diegote...\n");
}

// Fin del ejemplo. Principio del codigo


int opcionElegida;
char bufferName[FORMAT_NAME];
char bufferLastName[FORMAT_NAME];
char bufferCI[FORMAT_CI];

// Declaramos variables
Estudiante_t *newStudent; // La usamos para crear nuevo elemento en la lista de estidiantes. Puntero "Siguiente" apunta a NULL.
//Estudiante_t *listFirst; Esta declarada en el main. Confirmar en donde debe declararse.
Estudiante_t *listBuffer; // Puntero auxiliar para iterar en la lista.

//AGREGAR NUEVO ESTUDIANTE
// INICIALIZAR VARIABLES
Estudiante_t* inicializarEstudiante() { 
    Estudiante_t *newStudentPtr = malloc(sizeof(Estudiante_t)); //Reservamos memoria para un estudiante
    // Inicializar campos con strcpy (para cadenas)
    strcpy(newStudentPtr->nombre, ""); // Inicializamos vacio
    strcpy(newStudentPtr->apellido, ""); // Inicializamos vacio
    strcpy(newStudentPtr->CI, ""); // Inicializamos vacio
    // Inicializar campos enteros directamente
    newStudentPtr->grado = 0; // Inicializamos 0
    newStudentPtr->promCalif = 0; // Inicializamos 0
    // Inicializar puntero siguiente a NULL. Cerrar la lista
    newStudentPtr->siguiente = NULL;
    return newStudentPtr;
}
// CARGAR DATOS NUEVO ESTUDIANTE
Estudiante_t *cargarDatosMenu(Estudiante_t *auxPtr) { // Ver tipo de variable
    printf("Ingrese el nombre del estudiante: ");
    scanf("%s", &auxPtr->nombre);

    printf("Ingrese el apellido del estudiante: ");
    scanf("%s", &auxPtr->apellido);

    printf("Ingrese el CI del estudiante: ");
    scanf("%s", &auxPtr->CI);

    printf("Ingrese el grado del estudiante: ");
    scanf("%d", &auxPtr->grado);

    printf("Ingrese el promedio de calificaciones del estudiante: ");
    scanf("%d", &auxPtr->promCalif);

    // Imprimir los datos del estudiante (opcional)
    printf("\n----------------------------------------\n");
    printf("Datos del estudiante:\n");
    printf("Nombre: %s\n", auxPtr->nombre);
    printf("Apellido: %s\n", auxPtr->apellido);
    printf("CI: %s\n", auxPtr->CI);
    printf("Grado: %d\n", auxPtr->grado);
    printf("Promedio: %d\n", auxPtr->promCalif);
    printf("----------------------------------------\n");
    return auxPtr;
}
// BUSCAR ULTIMO EN LISTA
Estudiante_t *buscaUltimoLista(Estudiante_t *listPtr) {// Se le da como entrada el puntero al inicio de la lista
    Estudiante_t *auxPtr = listPtr; //Declaro e inicializo un puntero auxiliar que apunta al inicio de la lista.
    while (auxPtr->siguiente != NULL) { // Esta condicion se da cuando se llega al elemento final de la lista.
        auxPtr = auxPtr->siguiente; // Se itera apuntando el puntero auxiliar a la siguiente posicion
    }
    return auxPtr; //Devolvemos el puntero para que lo tome otra funcion
}


//DISPLAY LISTA/
//PERMUTAR 2 ELEMENTOS CONTIGUOS DE LA LISTA SEGUN CAMPO
// COMPARAR NOMBRE PARA REORDENAR ELEMENTOS DE LISTA
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
    free(nextPtr);
    return sortResult;
}
// COMPARAR APELLIDO PARA REORDENAR ELEMENTO DE LISTA
Sort_t sortStudentsLastNames(Estudiante_t *actualPtr, int sortCounter) { // Odrnea alfabeticamente y devuelve el siguiente elemento
    Estudiante_t *nextPtr = actualPtr->siguiente; // Apunto a los dos primero elementos para comparar
    int ordenLastName = strcmp(actualPtr->apellido,nextPtr->apellido); // str1 > str2 es positivo, str1 = str2 es 0, str1 < str2 es negativo
    if (ordenLastName > 0) { // Permutar lugares
        actualPtr->siguiente = nextPtr->siguiente;
        nextPtr->siguiente = actualPtr;
        actualPtr = nextPtr;// Pasar a siguiente estudiante
        sortCounter++;
    } else if (ordenLastName < 0) { 
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
    free(nextPtr);
    return sortResult;
}
// COMPARAR CI PARA REORDENAR ELEMENTO DE LA LISTA
Sort_t sortStudentsCI(Estudiante_t *actualPtr, int sortCounter) { // Odrnea alfabeticamente y devuelve el siguiente elemento
    Estudiante_t *nextPtr = actualPtr->siguiente; // Apunto a los dos primero elementos para comparar
    int ordenLastName = strcmp(actualPtr->CI,nextPtr->CI); // str1 > str2 es positivo, str1 = str2 es 0, str1 < str2 es negativo
    if (ordenLastName > 0) { // Permutar lugares
        actualPtr->siguiente = nextPtr->siguiente;
        nextPtr->siguiente = actualPtr;
        actualPtr = nextPtr;// Pasar a siguiente estudiante
        sortCounter++;
    } else { 
        actualPtr = nextPtr;// No hacer nada. Pasar a siguiente estudiante
    } 
    Sort_t sortResult = {actualPtr, sortCounter};
    free(nextPtr);
    return sortResult;
}
//REORDENAR LISRA SEGUN CAMPO ELEGIDO
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
    } while (sortResult.sortCounter != 0);// Realiza esta iteracion hasta que ya no queden mas permutaciones por realizar
    free(nextPtr);
    free(actualPtr);
    return;
}
// SORTEAR LISTA POR APELLIDO
void sortListByLastNames(Estudiante_t *listPtr) { // Odrnea alfabeticamente y devuelve el siguiente elemento
    Estudiante_t *actualPtr = listPtr; // Apunto a los dos primeros elementos para comparar
    Estudiante_t *nextPtr = actualPtr->siguiente;
    int sortCounter = 0;
    Sort_t sortResult = {actualPtr, sortCounter};
    do
    {
        sortResult.sortCounter = 0;
        while (nextPtr != NULL) {
            sortResult = sortStudentsLastNames(sortResult.actualPtr, sortResult.sortCounter);
        }
    } while (sortResult.sortCounter != 0); // Realiza esta iteracion hasta que ya no queden mas permutaciones por realizar
    free(nextPtr);
    free(actualPtr);
    return;
}
// SORTEAR LISTA POR CI
void sortListByCI(Estudiante_t *listPtr) { // Odrnea alfabeticamente y devuelve el siguiente elemento
    Estudiante_t *actualPtr = listPtr; // Apunto a los dos primeros elementos para comparar
    Estudiante_t *nextPtr = actualPtr->siguiente;
    int sortCounter = 0;
    Sort_t sortResult = {actualPtr, sortCounter};
    do
    {
        sortResult.sortCounter = 0;
        while (nextPtr != NULL) {
            sortResult = sortStudentsCI(sortResult.actualPtr, sortResult.sortCounter);
        }
    } while (sortResult.sortCounter != 0); // Realiza esta iteracion hasta que ya no queden mas permutaciones por realizar
    free(nextPtr);
    free(actualPtr);
    return;
}
//APUNTAR AL COMIENZO DE LA LISTA
// HACER LISTA ORDENADA CIRCULAR
void makeListCircular(Estudiante_t *listPtr) {
    Estudiante_t *actualPtr = listPtr; // Apunto a elemento actual
    while (actualPtr->siguiente != NULL) {
        actualPtr = actualPtr->siguiente;
    }
    actualPtr->siguiente = listPtr;
    free(actualPtr);
    return;
}
// APUNTAR A PRIMER ELEMENTO DE LISTA CIRCULAR ORDENADA  Buscando el menor NOMBRE y "CORTAR" LA COLA
Estudiante_t  *pointFirstName(Estudiante_t *listPtr) {// Asumimos que la lista ya es circular. Busca el primer elemento (mas chico) y lo apunta
    Estudiante_t *actualPtr =  listPtr;
    while (strcmp(actualPtr->nombre,actualPtr->siguiente->nombre) < 0)
    {
        actualPtr = actualPtr->siguiente; // Voy al siguiente
    }
    listPtr = actualPtr->siguiente; // La lista apunta al elemento mas chico. Osea, al primero alfabeticamente.
    actualPtr->siguiente = NULL;// El final de la lista se abre y apunta a NULL

    free(actualPtr);
    return listPtr;
}
// APUNTAR A PRIMER ELEMENTO DE LISTA CIRCULAR ORDENADA  Buscando el menor APELLIDO y "CORTAR" LA COLA
Estudiante_t  *pointFirstLastName(Estudiante_t *listPtr) {// Asumimos que la lista ya es circular. Busca el primer elemento (mas chico) y lo apunta
    Estudiante_t *actualPtr =  listPtr;
    while (strcmp(actualPtr->apellido,actualPtr->siguiente->apellido) < 0)
    {
        actualPtr = actualPtr->siguiente; // Voy al siguiente
    }
    listPtr = actualPtr->siguiente; // La lista apunta al elemento mas chico. Osea, al primero alfabeticamente.
    actualPtr->siguiente = NULL;// El final de la lista se abre y apunta a NULL

    free(actualPtr);
    return listPtr;
}
// APUNTAR A PRIMER ELEMENTO DE LISTA CIRCULAR ORDENADA  Buscando el menor CI y "CORTAR" LA COLA
Estudiante_t  *pointFirstCI(Estudiante_t *listPtr) {// Asumimos que la lista ya es circular. Busca el primer elemento (mas chico) y lo apunta
    Estudiante_t *actualPtr =  listPtr;
    while (strcmp(actualPtr->CI,actualPtr->siguiente->CI) < 0)
    {
        actualPtr = actualPtr->siguiente; // Voy al siguiente
    }
    listPtr = actualPtr->siguiente; // La lista apunta al elemento mas chico. Osea, al primero alfabeticamente.
    actualPtr->siguiente = NULL;// El final de la lista se abre y apunta a NULL

    free(actualPtr);
    return listPtr;
}
//DISPLAY LISTA
// Encabezado de la lista
void printListHeader() {
    printf("    APELLIDO   |    NOMBRE     |   CI   | GRADO |CALIFICACION\n");
    printf("-------------------------------------------------------------\n");
    return;
}
// Fila de la lista

void printStudentRow(Estudiante_t *actualPtr) {
    printf("%*s|%*s|%*s|%*s|%*s\n",FORMAT_NAME, actualPtr->apellido, FORMAT_NAME, actualPtr->nombre, FORMAT_CI, actualPtr->CI, 1, actualPtr->grado, 3, actualPtr->promCalif );
    return;
}

// Imprimir lista entera
void displayList(Estudiante_t *listPtr) {
    Estudiante_t *actualPtr = listPtr; // Apunto a elemento actual
    // Contemplar lista vacia y de un solo elemento.
    if (actualPtr == NULL) {
        printf("La lista esta vacia. Desea agregar un nuevo estudiante?\n");
    }else if (actualPtr->siguiente = NULL) {
        printListHeader();
        printStudentRow(actualPtr);
    }else{
        printListHeader();
        do
        {
            printStudentRow(actualPtr);
            actualPtr = actualPtr->siguiente; // Iteramos en la lista.
        } 
        while (actualPtr->siguiente != NULL);
    }
    free(actualPtr);
    return;
}

//BORRAR

// Borra estudiante seleccionado
void deleteStudent(Estudiante_t *actualPtr, Estudiante_t *listPtr) {
    if (actualPtr == listPtr) { //Caso 1: El elemento se encuentra justo al inicio de la lista.
        listPtr = listPtr->siguiente; // Si listPtr->siguiente = NULL, queda la lista nula.
        free(actualPtr);
    }else{
        Estudiante_t *auxPtr = listPtr; // Apunto al principio de la lista para iterar
        makeListCircular(listPtr); // Hago la lista circular para poder recorrerla y llegar al elemento anterior de actualPtr
        while (auxPtr->siguiente != actualPtr) { //Busco pararme en el elemento anterior al senialado por actualPtr
            auxPtr = auxPtr->siguiente;
        }
        auxPtr->siguiente = actualPtr->siguiente; // "Bypass" en la lista. Ahora solo debo eliminar actualPtr
        free(actualPtr);
        free(auxPtr);
    }
    return;
}
// Da opciones para borrar estudiante encontrado
void deleteStudentOptions(Estudiante_t *actualPtr, Estudiante_t *listPtr) {
    char confirm;
    printf("Desea eliminar estudiante? Y/N: \n");
            do
            {
                scanf("%s", &confirm);
                switch (confirm)
                {
                case 'Y':
                    deleteStudent(actualPtr, listPtr);
                    printf("Estudiante borrado correctamente.\n");
                    confirm = 'N';
                    break;
                case 'N':
                    printf("Saliendo...\n");
                    break;
                default:
                    printf("Parametro desconocido. Ingrese nuevamente.\n");
                    printf("Desea eliminar estudiante? Y/N: \n");
                    break;
                }
            }
            while (confirm != 'N'); //Solo sale en caso de detectar N o en caso de entrar a Y y ejecutar codigo.
}
// Las busquedas se realizan iterando toda la lista enlazada hasta llegar a NULL
void deleteStudentMenu(Estudiante_t *listPtr) {
    int matchCounter = 0;
    Estudiante_t *actualPtr = listPtr; // Apunto a elemento actual
    printf("Ingrese la CI del estudiante a eliminar: ");
    
    scanf("%*s",FORMAT_CI, &bufferCI);
    if (actualPtr == NULL) {
        printf("No se han obtenido resultados.\n");
    }else if (actualPtr->siguiente = NULL) {
        if (actualPtr->CI == bufferCI) {
            printf("Resultado de busqueda:\n");
            printListHeader();
            printStudentRow(actualPtr);
            deleteStudentOptions(actualPtr, listPtr);
        }else {
            printf("No se han obtenido resultados.\n");
        }
    }else{
            do
            {
                if (actualPtr->CI == bufferCI) {
                    printf("Resultado de busqueda:\n");
                    printListHeader();
                    printStudentRow(actualPtr);
                    deleteStudentOptions(actualPtr, listPtr);
                }
                actualPtr = actualPtr->siguiente; // Iteramos en la lista.
            }
            while (actualPtr->siguiente != NULL); // Evalua primero si hay match y luego si llego al final
            printf("No se han obtenido resultados.\n");
        }
        free(actualPtr);
    return;
}



//FUNCIONES DE MENU
void accesoMenu(Estudiante_t *listPtr, Estudiante_t *resultPtr) {
    int flagMenu = 0;
    do {
        //Muestra las opciones del Menu
        printf("\n---------Sistema de gestion de estudiantes de IoT---------\n");
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
                displayListMenu(listPtr);
                break;
            case '3':
                deleteStudentMenu(listPtr);
                break;
            case '4':
                printf("Saliendo...");
                break;
            default:
                printf("Opcion invalida. Ingrese nuevamente.\n");
        }
    }
    while (opcionElegida != 4);
    return;
}

void addNewStudentMenu(Estudiante_t *listPtr) { // Toma como entrada el puntero al primer elemento de la lista.
    do
    {   
        printf("Desea agregar un nuevo estudiante?");
        printf("1. Si. -> Agregar datos");
        printf("2. No. -> Volver a menu principal");
        scanf("%d", &opcionElegida);
        switch (opcionElegida) {
            case '1':{
                Estudiante_t *newStudent = inicializarEstudiante();// Inicializamos el nuevo elemento.

                newStudent = cargarDatos(newStudent);// Cargamos los datos del estudiante.
                Estudiante_t *ultimoList = buscaUltimoLista(listPtr);// Apuntamos al ultimo lugar de la lista.
                ultimoList->siguiente = newStudent;// "Enganchamos" nuevo elemento a la lista. newStudent->siguiente ya es NULL por inicializacion
                free(ultimoList);
                free(newStudent);
                }
                break;
            case '2':
                printf("Saliendo...");
                break;
            default:
                printf("Opcion invalida. Ingrese nuevamente.\n");
        }
    } while (opcionElegida != 2);
    return;
}

void displayListMenu(Estudiante_t *listPtr) { // Toma como entrada el puntero al primer elemento de la lista.
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
            sortListByNames(listPtr); // Ordena por nombres
            makeListCircular(listPtr); // Crea lista circular
            listPtr = pointFirstName(listPtr); // El comienzo de la lista es el valor mas chico
            displayList(listPtr);
                break;
            case '2':
            sortListByLastNames(listPtr); // Ordena por apellido
            makeListCircular(listPtr); // Crea lista circular
            listPtr = pointFirstLastName(listPtr); // El comienzo de la lista es el valor mas chico
            displayList(listPtr);
                break;
            case '3':
            sortListByCI(listPtr); // Ordena por CI
            makeListCircular(listPtr); // Crea lista circular
            listPtr = pointFirstCI(listPtr); // El comienzo de la lista es el valor mas chico
            displayList(listPtr);
                break;
            case '4':
                printf("Saliendo...\n");
                break;
            default:
                printf("Opcion invalida. Ingrese nuevamente.\n");
        }
    } while (opcionElegida != 4);
    return;
}
// ESTOY TRABAJANDO EN EL MENU DE ELIMINACION DE ESTUDIANTE