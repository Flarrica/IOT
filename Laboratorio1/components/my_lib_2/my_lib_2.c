#include <stdio.h>
#include "my_lib_2.h"

//---------------------------------------------------VALIDACION de datos y ERRORES----------------------------------------------------------//
// Validacion de nuevo estudiante
CodigoError_t validacionNuevoEstudiante (char nombre[FORMAT_NAME], char apellido[FORMAT_LASTNAME], char CI[FORMAT_CI], Grado_t grado, int promCalif) {
    CodigoError_t error;
    if (strlen(nombre) >= FORMAT_NAME) {// Verifica largo del Nombre
        error = ERROR_NOMBRE_LARGO;
    }
    if (strlen(apellido) >= FORMAT_LASTNAME) {// Verifica largo del Apellido
        error = ERROR_APELLIDO_LARGO;
    }
    if (strlen(CI) >= FORMAT_CI) {// Verifica largo del CI
        error = ERROR_CI_LARGO;
    }

    if (grado < 1 || grado > 6) {// Verifica rango del grado. Asumimos liceo, entonces es de primero a sexto.
        return ERROR_GRADO_RANGO;
    }
    if (promCalif  < 0 || promCalif > 100) {// Verifica rango de la calificacion. del 0 al 100;
        return ERROR_CALIFICACIONES_RANGO;
    }
    error = NO_ERROR;
    return error;
}
void printErrorNuevoEstudiante(CodigoError_t error) {
    if (error != NO_ERROR) { // Realizamos print para cada error
        switch (error) {
            case ERROR_NOMBRE_LARGO:
                printf("Error: Nombre demasiado largo.\n");
                break;
            case ERROR_APELLIDO_LARGO:
                printf("Error: Apellido demasiado largo.\n");
                break;
            case ERROR_CI_LARGO:
                printf("Error: CI demasiado largo.\n");
                break;
            case ERROR_GRADO_RANGO:
                printf("Error: Grado fuera de rango.\n");
                break;
                // Agrega más casos para otros errores...
            default:
                printf("Error desconocido.\n");
        }
    }
    return;
}

//Validacion filtro de lista
CodigoError_t validacionFiltroLista (char nombre[FORMAT_NAME], char apellido[FORMAT_LASTNAME], char CI[FORMAT_CI], Grado_t grado, int promCalif) {
    CodigoError_t error;
    if (strlen(nombre) >= FORMAT_NAME) {// Verifica largo del Nombre
        error = ERROR_NOMBRE_LARGO;
    }
    if (strlen(apellido) >= FORMAT_LASTNAME) {// Verifica largo del Apellido
        error = ERROR_APELLIDO_LARGO;
    }
    if (strlen(CI) >= FORMAT_CI) {// Verifica largo del CI
        error = ERROR_CI_LARGO;
    }

    if (grado < 1 || grado > 6) {// Verifica rango del grado. Asumimos liceo, entonces es de primero a sexto.
        return ERROR_GRADO_RANGO;
    }
    if (grado < 0 || grado > 100) {// Verifica rango de la calificacion. del 0 al 100;
        return ERROR_CALIFICACIONES_RANGO;
    }
    error = NO_ERROR;
    return error;
}
void printErrorFiltroLista(CodigoError_t error) {
    if (error != NO_ERROR) { // Realizamos print para cada error
        switch (error) {
            case ERROR_NOMBRE_LARGO:
                printf("Error: Nombre demasiado largo.\n");
                break;
            case ERROR_APELLIDO_LARGO:
                printf("Error: Apellido demasiado largo.\n");
                break;
            case ERROR_CI_LARGO:
                printf("Error: CI demasiado largo.\n");
                break;
            case ERROR_GRADO_RANGO:
                printf("Error: Grado fuera de rango.\n");
                break;
                // Agrega más casos para otros errores...
            default:
                printf("Error desconocido.\n");
        }
    }
    return;
}
//-----------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------AUXILIARES de funcion prinicpal: AGREGAR NUEVO ESTUDIANTE------------------------------------//
// BUSCAR ULTIMO EN LISTA
Estudiante_t *ultimoElementoLista(Estudiante_t *listPtr) {// Se le da como entrada el puntero al inicio de la lista
    //printf("Entra funcion auxiliar - BUSCAR_ULTIMO_LISTA\n");
    Estudiante_t *actualPtr = listPtr; //Declaro e inicializo un puntero auxiliar que apunta al inicio de la lista.
    while (actualPtr->siguiente != NULL) { // Esta condicion se da cuando se llega al elemento final de la lista.
        actualPtr = actualPtr->siguiente; // Se itera apuntando el puntero auxiliar a la siguiente posicion
    }
    //printf("Sale funcion auxiliar - BUSCAR_ULTIMO_LISTA\n");
    return actualPtr; //Devolvemos el puntero para que lo tome otra funcion
}
//-----------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------AUXILIARES de funcion prinicpal: DISPLAY LISTA------------------------------------------------//

// COMPARAR ELEMENTOS CONTIGUOS Y PERMUTAR EN CASO DE SER NECESARIO SEGUN FILTRO
Sort_t sortElementsList(Estudiante_t *actualPtr, int sortCounter, SortBy_t filtro) { // Odrnea alfabeticamente y devuelve el siguiente elemento
    //printf("->DISPLAY->sortList->sortElementList: Entra\n");
    Estudiante_t *nextPtr = actualPtr->siguiente; // Apunto a los dos primero elementos para comparar
    int orden = 0; // str1 > str2 es positivo, str1 = str2 es 0, str1 < str2 es negativo
    if (filtro != SIN_FILTRO)
    {
        switch (filtro) { // Da como resultado el orden comparativo entre los dos elementos dependiendo de como estan sorteados
            case NOMBRE:
            orden = strcmp(actualPtr->nombre,nextPtr->nombre);
                break;
            case APELLIDO:
            orden = strcmp(actualPtr->apellido,nextPtr->apellido);
                break;
            case CI:
            orden = strcmp(actualPtr->CI,nextPtr->CI);
                break;
            default:
            break;
        }
    }
    if (orden > 0) { // Permutar lugares dependiendo del orden, que depende del sortby (nombre, apellido, CI)
        actualPtr->siguiente = nextPtr->siguiente;
        nextPtr->siguiente = actualPtr;
        actualPtr = nextPtr;// Pasar a siguiente estudiante
        sortCounter++;
    } else if (orden < 0) { 
        actualPtr = nextPtr;// No hacer nada. Pasar a siguiente estudiante
    } else if (filtro != CI)// Sortear por cedula ascendente en caso de empate en nombre o apellido
    {
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
    //printf("->DISPLAY->sortList->sortElementList: Sale\n");
    return sortResult;
}
// ORDENA TODA LA LISTA SEGUN EL FILTRO INDICADO
void sortList(Estudiante_t *listPtr, SortBy_t filtro) {
    //printf("->DISPLAY->sortList: Entra\n");
    Estudiante_t *actualPtr = listPtr; // Apunto a los dos primeros elementos para comparar
    int sortCounter = 0;
    //printf("->DISPLAY->sortList: Flag 1\n");
    Sort_t sortResult = {actualPtr, sortCounter};
    //printf("->DISPLAY->sortList: Flag 2\n");
    do
    {
        //printf("->DISPLAY->sortList: Flag 3\n");
        sortResult.sortCounter = 0;
        //printf("->DISPLAY->sortList: Flag 4\n");
        //printf("Iterando nombre: %s\n", listPtr->nombre);

       // printf("Valor de puntero siguiente al actual: %s\n", actualPtr->nombre);
        while (actualPtr->siguiente != NULL) {
            //printf("->DISPLAY->sortList: Flag 5\n");
           // printf("Entra while. Iteracion: %d\n", sortResult.sortCounter);
            sortResult = sortElementsList(sortResult.actualPtr, sortResult.sortCounter, filtro);
        }
    } while (sortResult.sortCounter != 0);// Realiza esta iteracion hasta que ya no queden mas permutaciones por realizar
   // printf("->DISPLAY->sortList: Flag 5\n");
    free(actualPtr);
    //printf("->DISPLAY->sortList: Sale\n");
    return;
}
// HACER LISTA ORDENADA CIRCULAR
void makeListCircular(Estudiante_t *listPtr) {
    printf("->DISPLAY->makeCircular: Entra\n");
    Estudiante_t *actualPtr = listPtr; // Apunto a elemento actual
    while (actualPtr->siguiente != NULL) {
        actualPtr = actualPtr->siguiente;
    }
    actualPtr->siguiente = listPtr;
    free(actualPtr);
    printf("->DISPLAY->makeCircular: Sale\n");
    return;
}
// APUNTAR A PRIMER ELEMENTO DE LISTA CIRCULAR ORDENADA / Segun filtro elegido
Estudiante_t *pointFirstElementBySort(Estudiante_t *listPtr, SortBy_t filtro) {// Asumimos que la lista ya es circular. Busca el primer elemento (mas chico) y lo apunta
    printf("->DISPLAY->pointFirstElementBySort: Entra\n");
    Estudiante_t *actualPtr =  listPtr;
    if (filtro != SIN_FILTRO)
    {
        makeListCircular(listPtr);
        switch (filtro) {
            case NOMBRE:
            while (strcmp(actualPtr->nombre,actualPtr->siguiente->nombre) < 0)
            {
                actualPtr = actualPtr->siguiente; // Voy al siguiente
            }
                break;
            case APELLIDO:
            while (strcmp(actualPtr->apellido,actualPtr->siguiente->apellido) < 0)
            {
                actualPtr = actualPtr->siguiente; // Voy al siguiente
            }
                break;
            case CI:
            while (strcmp(actualPtr->CI,actualPtr->siguiente->CI) < 0)
            {
                actualPtr = actualPtr->siguiente; // Voy al siguiente
            }
                break;
            default:
            break;
        }
        listPtr = actualPtr->siguiente; // La lista apunta al elemento mas chico. Osea, al primero alfabeticamente.
        actualPtr->siguiente = NULL;// El final de la lista se abre y apunta a NULL
    }
    free(actualPtr);
    printf("->DISPLAY->pointFirstElementBySort: Sale\n");
    return listPtr;
}
// PRINTF - Encabezado de la lista
void printListHeader() {
    printf("    APELLIDO   |    NOMBRE     |   CI   | GRADO |CALIFICACION\n");
    printf("-------------------------------------------------------------\n");
    return;
}
// PRINTF - Fila de la lista
void printStudentRow(Estudiante_t *actualPtr) {
    printf("->DISPLAY->printfList->printStudentRow: Entra\n");
    printf("%s|%s|%s|%d|%u\n", actualPtr->apellido,  actualPtr->nombre, actualPtr->CI, actualPtr->grado, actualPtr->promCalif );
    printf("->DISPLAY->printfList->printStudentRow: Sale\n");
    return;
}
// PRINTF - Imprimir lista entera
void printfList(Estudiante_t *listPtr) {
    printf("->DISPLAY->printfList: Entra\n");
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
    printf("->DISPLAY->printfList: Sale\n");
    return;
}
//--------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------AUXILIARES de funcion prinicpal: BORRAR ELEMENTO-------------------------------------------------//

// Borra estudiante seleccionado
void deleteStudentElement(Estudiante_t *actualPtr, Estudiante_t *listPtr) {
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
                    deleteStudentElement(actualPtr, listPtr);
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


//FUNCIONES PRINCIPALES
// Esta funcion toma como entradas los datos de un estudiante y lo agrega a la lista
Estudiante_t *agregarNuevoEstudiante(Estudiante_t *listPtr, char nombre[FORMAT_NAME], char apellido[FORMAT_LASTNAME], char CI[FORMAT_CI], Grado_t grado, int promCalif) { 
    //VALIDACION DE ERRORES
    printf("Entra funcion principal - AGREGAR\n");
    CodigoError_t error = validacionNuevoEstudiante (nombre, apellido, CI, grado, promCalif);
    if (error != NO_ERROR) // SI HAY ERROR, IMPRIME EN PANTALLA CUAL ES Y ABORTA LA FUNCION. SI NO HAY ERROR< SIGUE.
    {
        printErrorNuevoEstudiante(error);
        return listPtr;
    }
    // CUERPO DE LA FUNCION
    Estudiante_t *newStudentPtr = malloc(sizeof(Estudiante_t)); //Reservamos memoria para un estudiante
 
    // Cargar datos por campo.
    printf("Comienza copia de strings\n");
    strcpy(newStudentPtr->nombre, nombre);
    strcpy(newStudentPtr->apellido, apellido);
    strcpy(newStudentPtr->CI, CI);
    printf("Pasa la parte de copia de strings\n");
    newStudentPtr->grado = grado;
    newStudentPtr->promCalif = (unsigned char)promCalif;
    newStudentPtr->siguiente = NULL;
    printf("Print Student:\n");
    printStudentRow(newStudentPtr);
    free(newStudentPtr);
    printf("Sale funcion principal - AGREGAR\n");

    if (listPtr == NULL)
    {
        printf("Entra cuando la lista es nula\n");
        return newStudentPtr;
    }else
    {
        printf("Entra cuando la lista no es nula\n");
        Estudiante_t *ulitmoElemPtr = ultimoElementoLista(listPtr);
        ulitmoElemPtr->siguiente = newStudentPtr; // Enlazamos al final de la lista.
        printf("Pasa asignacion de puntero newStudent\n");
        free(ulitmoElemPtr);
        printf("Pasa liberacion de ultimoElemPtr\n");
        return listPtr;
    }
    
}

void displayList(Estudiante_t *listPtr, SortBy_t filtro) { // Toma como entrada el puntero al primer elemento de la lista.
    printf("Entra funcion principal - DISPLAY\n");
    sortList(listPtr, filtro); // Ordena la lista segun el filtro seleccionado
    if (filtro != SIN_FILTRO)
    {
        makeListCircular(listPtr); // Cierra la lista haciendola circular
        listPtr = pointFirstElementBySort(listPtr, filtro); //Mueve el puntero de lista al primer lugar y corta la lista nuevamente
    }
    printfList(listPtr);
    printf("Sale funcion principal - DISPLAY\n");
    return;
}

void deleteStudentFromList(Estudiante_t *listPtr, char CI[FORMAT_CI]) {
    printf("Entra funcion principal - BORRAR\n");
    int matchCounter = 0;
    Estudiante_t *actualPtr = listPtr; // Apunto a elemento actual
    if (actualPtr == NULL) {
        printf("La lista esta vacia. Imposible borrar.\n");
    }else if (actualPtr->siguiente == NULL) {
        if (strcmp(actualPtr->CI,CI) == 0) {
            printListHeader();
            printStudentRow(actualPtr);
            deleteStudentOptions(actualPtr, listPtr);
        }else {
            printf("No se han obtenido resultados.\n");
        }
    }else{
            do
            {
                if (strcmp(actualPtr->CI,CI) == 0) {
                    printListHeader();
                    printStudentRow(actualPtr);
                    deleteStudentOptions(actualPtr, listPtr);
                }
                actualPtr = actualPtr->siguiente; // Iteramos en la lista.
            }
            while (actualPtr->siguiente != NULL); // Evalua primero si hay match y luego si llego al final
        }
        free(actualPtr);
        printf("Sale funcion principal - BORRAR\n");
    return;
}