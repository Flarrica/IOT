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



Estudiante_t *buscaMenorElemento(Estudiante_t *listPtr, SortBy_t filtro) {
    printf("->buscaMenorElemento: Entra\n");
    
    if (listPtr == NULL) {
        printf("Lista vacía.\n");
        return NULL;
    }
    Estudiante_t *actualPtr = listPtr;
    Estudiante_t *sortedElemPtr = listPtr;
    char sortedName[FORMAT_NAME];
    strcpy(sortedName, listPtr->nombre);
    printf("sortedName es: %s\n", sortedName);
    char sortedLastname[FORMAT_LASTNAME];
    strcpy(sortedLastname, listPtr->apellido);
    printf("sortedLastname es: %s\n", sortedLastname);
    char sortedCI[FORMAT_CI];
    strcpy(sortedCI, listPtr->CI);
    printf("sortedCI es: %s\n", sortedCI);
    int orden = 0; // El orden asignado a dos elementos, comparando por el filtro indicado en la funcion
    printf("->buscaMenorElemento:  filtros Entra\n");
    if (filtro != SIN_FILTRO)
    {
        switch (filtro) {
            case NOMBRE:
            strcpy(sortedName, listPtr->nombre); // Apuntamos al primer nombre de la lista
            printf("El nombre en la variable sortedName es %s: \n", sortedName);
            printf("El nombre en la variable listPtr->nombre es %s: \n", listPtr->nombre);
            printf("->buscaMenorElemento:  pasa el strcpy\n");
            while (actualPtr->siguiente != NULL){
                //printf("->buscaMenorElemento: Entra al while\n");
                orden = strcmp(sortedName, actualPtr->siguiente->nombre);// Compara el primer nombre guardado con el segundo
                if (orden >0) // Caso: Primer nombre Bruno. Segundo nombre Alma. Me quedo con el segundo como el menor.
                {
                    sortedElemPtr = sortedElemPtr->siguiente; // Ahora apunta al siguiente elemento.
                    strcpy(sortedName, sortedElemPtr->siguiente->nombre); // Copia el nombre en la variable string auxiliar para comparar.
                }
                actualPtr = actualPtr->siguiente;
            } // A la salida de esto tengo el puntero sortedElemPtr que apunta al valor mas chico de todos.
                break;
            case APELLIDO:
            strcpy(sortedLastname, listPtr->apellido); // Apuntamos al primer apellido de la lista
            while (actualPtr->siguiente != NULL){
                orden = strcmp(sortedLastname, actualPtr->siguiente->apellido);// Compara el primer apellido guardado con el segundo
                if (orden >0) // Caso: Primer apellido Zuñiga. Segundo apellido Alvarez. Me quedo con el segundo como el menor.
                {
                    sortedElemPtr = sortedElemPtr->siguiente; // Ahora apunta al siguiente elemento.
                    strcpy(sortedLastname, sortedElemPtr->siguiente->apellido); // Copia el apellido en la variable string auxiliar para comparar.
                }
                actualPtr = actualPtr->siguiente;
            } // A la salida de esto tengo el puntero sortedElemPtr que apunta al valor mas chico de todos.
                break;
            case CI:
            strcpy(sortedCI, listPtr->CI); // Apuntamos al primer CI de la lista
            while (actualPtr->siguiente != NULL){
                orden = strcmp(sortedCI, actualPtr->siguiente->CI);// Compara el primer CI guardado con el segundo
                if (orden >0) // Caso: Primer CI 43748445. Segundo CI 34748445. Me quedo con el segundo como el menor.
                {
                    sortedElemPtr = sortedElemPtr->siguiente; // Ahora apunta al siguiente elemento.
                    strcpy(sortedCI, sortedElemPtr->siguiente->CI); // Copia el CI en la variable string auxiliar para comparar.
                }
                actualPtr = actualPtr->siguiente;
            } // A la salida de esto tengo el puntero sortedElemPtr que apunta al valor mas chico de todos.
                break;
            default:
            break;
        }  // A la salida de esto tengo el puntero sortedElemPtr que apunta al valor mas chico de todos.
    }
    printf("->buscaMenorElemento: Sale\n");
    return sortedElemPtr;
}

Estudiante_t *armarListaOrdenada(Estudiante_t *listPtr, SortBy_t filtro) {
    printf("->armarListaOrdenada: Entra\n");
    if (listPtr == NULL)
    {
        printf("La lista esta vacia.\n");
        return listPtr;
    }else if (listPtr->siguiente == NULL)
    {
        printf("La lista ya se encuentra ordenada.\n");
        return listPtr;
    }
    Estudiante_t *actualPtr = listPtr;
    Estudiante_t *previousPtr = listPtr;
    Estudiante_t *sortedElementPtr = NULL; // La lista ordenada arranca con el menor elemento.
    Estudiante_t *ultimoSortedList = NULL;
    Estudiante_t *sortedListPtr = NULL;
    do
    {
        //printf("->armarListaOrdenada: Entra al DO\n");
        sortedElementPtr = buscaMenorElemento(listPtr, filtro);
        actualPtr = sortedElementPtr;
        while (previousPtr->siguiente != sortedElementPtr) //Busca pararse en el elemento anterior al actual
        {
            printf("Entro a while, siguiente nombre es: %s\n", previousPtr->siguiente->nombre);
            previousPtr = previousPtr->siguiente;
        } // Tengo el puntero actual apuntando al elemento sorteado y el previous al elemento anterior en la lista.
        previousPtr->siguiente = actualPtr->siguiente; // Engancho lista, dejando afuera el elemento actual
        if (sortedListPtr == NULL) // Seria como inicializar la lista sorteada
        {
            sortedListPtr = sortedElementPtr;
            sortedListPtr->siguiente = NULL;
            sortedElementPtr = NULL;
        }else{
            ultimoSortedList = ultimoElementoLista(sortedListPtr); // Busco el ultimo elemento de la lista ordenada
            ultimoSortedList->siguiente = sortedElementPtr; // Armo la lista
            ultimoSortedList->siguiente->siguiente = NULL; // Apunto el final a null
            sortedElementPtr = NULL;
            actualPtr = NULL; // Libero el puntero
        }
        previousPtr = listPtr;
    } while (listPtr->siguiente != NULL);
    printf("->armarListaOrdenada: Sale del DO\n");
    // Aca tengo que pasar el ultimo elemento que quedo suelto.
    ultimoSortedList = ultimoElementoLista(sortedListPtr);
    ultimoSortedList->siguiente = listPtr;
    ultimoSortedList->siguiente->siguiente = NULL; // Apunto el final a null
    listPtr = NULL;
    printf("->armarListaOrdenada: Sale\n");
    return sortedListPtr;
}



// COMPARAR ELEMENTOS CONTIGUOS Y PERMUTAR EN CASO DE SER NECESARIO SEGUN FILTRO
Sort_t *sortElementsList(Sort_t* sortResult, SortBy_t filtro) { // Odrnea alfabeticamente y devuelve el siguiente elemento
    printf("->DISPLAY->sortList->sortElementList: Entra\n");
    if (sortResult->actualPtr->siguiente == NULL)
    {
        sortResult->sortCounter = 0;
        return sortResult;
    }
    Estudiante_t *nextPtr = sortResult->actualPtr->siguiente; // Apunto a los dos primero elementos para comparar
    int orden; // str1 > str2 es positivo, str1 = str2 es 0, str1 < str2 es negativo
    printf("Primer nombre es: %s\n", sortResult->actualPtr->nombre);
    printf("Segundo nombre es: %s\n", nextPtr->nombre);
    if (filtro != SIN_FILTRO)
    {
        switch (filtro) { // Da como resultado el orden comparativo entre los dos elementos dependiendo de como estan sorteados
            case NOMBRE:
            
            orden = strcmp(sortResult->actualPtr->nombre,nextPtr->nombre);
            printf("Primer nombre es: %s\n", sortResult->actualPtr->nombre);
            printf("Segundo nombre es: %s\n", nextPtr->nombre);
            printf("Orden NOMBRE es: %d\n", orden);
                break;
            case APELLIDO:
            orden = strcmp(sortResult->actualPtr->apellido,nextPtr->apellido);
            printf("Orden APELLIDO es: %d\n", orden);
                break;
            case CI:
            orden = strcmp(sortResult->actualPtr->CI,nextPtr->CI);
            printf("Orden CI es: %d\n", orden);
                break;
            default:
            printf("Entra a default.\n");
            break;
        }
    }
    if (orden > 0) { // Permutar lugares dependiendo del orden, que depende del sortby (nombre, apellido, CI)
        sortResult->actualPtr->siguiente = nextPtr->siguiente;
        nextPtr->siguiente = sortResult->actualPtr;
        sortResult->actualPtr = nextPtr;// Pasar a siguiente estudiante
        sortResult->sortCounter++;
        printf("Entra orden > 0. Iteracion: %d\n", sortResult->sortCounter);
    } else if (orden < 0) { 
        sortResult->actualPtr = nextPtr;// No hacer nada. Pasar a siguiente estudiante
        printf("Entra orden < 0. Iteracion: %d\n", sortResult->sortCounter);
    } else{
        printf("Entra al else. Iteracion: %d\n", sortResult->sortCounter);
        if (filtro != CI)// Sortear por cedula ascendente en caso de empate en nombre o apellido
        {
            
        int ordenCI = strcmp(sortResult->actualPtr->CI,nextPtr->CI); // str1 > str2 es positivo, str1 = str2 es 0, str1 < str2 es negativo
        if (ordenCI > 0) { // Permutar lugares
            printf("Entra a comparador de cedulas con ordenCI > 0. Iteracion: %d\n", sortResult->sortCounter);
            sortResult->actualPtr->siguiente = nextPtr->siguiente;
            nextPtr->siguiente = sortResult->actualPtr;
            sortResult->actualPtr = nextPtr;// Pasar a siguiente estudiante
            sortResult->sortCounter++;
            printf("Entra ordenCI > 0. Iteracion: %d\n", sortResult->sortCounter);
        } else {
            sortResult->actualPtr = nextPtr; // No hacer nada. Pasar a siguiente estudiante
            printf("Entra a comparador de cedulas con ordenCI < 0. Iteracion: %d\n", sortResult->sortCounter);
            }
        }
    }
    //printf("->DISPLAY->sortList->sortElementList: Sale\n");
    nextPtr = NULL;
    return sortResult;
}
// ORDENA TODA LA LISTA SEGUN EL FILTRO INDICADO
void sortList(Estudiante_t *listPtr, SortBy_t filtro) {
    //printf("->DISPLAY->sortList: Entra\n");
    Estudiante_t *actualPtr = listPtr; // Apunto a los dos primeros elementos para comparar
    int sortCounter = 0;
    printf("->DISPLAY->sortList: Flag 1\n");
    Sort_t *sortResult = malloc(sizeof(Sort_t)); //Reservamos memoria para un estudiante
    sortResult->actualPtr = actualPtr;
    sortResult->sortCounter = 0;
    printf("->DISPLAY->sortList: Flag 2\n");
    do
    {
        //printf("->DISPLAY->sortList: Flag 3\n");
        sortResult->sortCounter = 0;
        //printf("->DISPLAY->sortList: Flag 4\n");
        //printf("Iterando nombre: %s\n", listPtr->nombre);

       //printf("Valor de puntero siguiente al actual: %s\n", actualPtr->nombre);
        while (sortResult->actualPtr->siguiente != NULL) {
            printf("->DISPLAY->sortList: Flag 5\n");
            sortResult = sortElementsList(sortResult, filtro);
            printf("->DISPLAY->sortList: Flag 6\n");
            sortResult->actualPtr = sortResult->actualPtr->siguiente;
            printf("->DISPLAY->sortList: Flag 7\n");
        }
        printf("Entra while. Iteracion: %d\n", sortResult->sortCounter);
    } while (sortResult->sortCounter != 0);// Realiza esta iteracion hasta que ya no queden mas permutaciones por realizar
   // printf("->DISPLAY->sortList: Flag 5\n");
    actualPtr = NULL;
    printf("->DISPLAY->sortList: Sale\n");
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
    actualPtr=NULL;
    printf("->DISPLAY->makeCircular: Sale\n");
    return;
}
// APUNTAR A PRIMER ELEMENTO DE LISTA CIRCULAR ORDENADA / Segun filtro elegido
Estudiante_t *pointFirstElementBySort(Estudiante_t *listPtr, SortBy_t filtro) {// Asumimos que la lista ya es circular. Busca el primer elemento (mas chico) y lo apunta
    printf("->DISPLAY->pointFirstElementBySort: Entra\n");
    Estudiante_t *actualPtr =  listPtr;
    if (filtro != SIN_FILTRO)
    {
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
    actualPtr = NULL;
    printf("->DISPLAY->pointFirstElementBySort: Sale\n");
    return listPtr;
}
// PRINTF - Encabezado de la lista
void printListHeader() {
    printf("Entra al print.\n");
    printf("%*s|%*s|%*s|%*s|%*s\n",FORMAT_NAME, "NOMBRE", FORMAT_LASTNAME, "APELLIDO", FORMAT_CI, "CI", FORMAT_GRADE, "GRADO", FORMAT_PROMCALIF, "CALIFICACION");
    printf("-------------------------------------------------------------\n");
    return;
}
// PRINTF - Fila de la lista
void printStudentRow(Estudiante_t *actualPtr) {
    //printf("->DISPLAY->printfList->printStudentRow: Entra\n");
    printf("%*s|%*s|%*s|%5d|%12d\n", FORMAT_NAME, actualPtr->nombre, FORMAT_LASTNAME, actualPtr->apellido, FORMAT_CI, actualPtr->CI, actualPtr->grado, actualPtr->promCalif );
    //printf("->DISPLAY->printfList->printStudentRow: Sale\n");
    return;
}
// PRINTF - Imprimir lista entera
void printfList(Estudiante_t *listPtr) {
    //printf("->DISPLAY->printfList: Entra\n");
    Estudiante_t *actualPtr = listPtr; // Apunto a elemento actual
    // Contemplar lista vacia y de un solo elemento.
    if (actualPtr == NULL) {
        printf("La lista esta vacia. Desea agregar un nuevo estudiante?\n");
    }else if (actualPtr->siguiente == NULL) {
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
    actualPtr=NULL;
    //printf("->DISPLAY->printfList: Sale\n");
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
        actualPtr=NULL;
        auxPtr=NULL;
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
    //printf("Entra funcion principal - AGREGAR\n");
    CodigoError_t error = validacionNuevoEstudiante (nombre, apellido, CI, grado, promCalif);
    if (error != NO_ERROR) // SI HAY ERROR, IMPRIME EN PANTALLA CUAL ES Y ABORTA LA FUNCION. SI NO HAY ERROR< SIGUE.
    {
        printErrorNuevoEstudiante(error);
        return listPtr;
    }
    // CUERPO DE LA FUNCION
    Estudiante_t *newStudentPtr = malloc(sizeof(Estudiante_t)); //Reservamos memoria para un estudiante
 
    // Cargar datos por campo.
    //printf("Comienza copia de strings\n");
    strcpy(newStudentPtr->nombre, nombre);
    //printf("newStudentPtr->nombre es: %s\n", newStudentPtr->nombre);
    //printf("nombre es: %s\n", nombre);
    strcpy(newStudentPtr->apellido, apellido);
    strcpy(newStudentPtr->CI, CI);
    //printf("Pasa la parte de copia de strings\n");
    newStudentPtr->grado = grado;
    newStudentPtr->promCalif = (unsigned char)promCalif;
    newStudentPtr->siguiente = NULL;
    //printf("Print Student:\n");
    //printStudentRow(newStudentPtr);
    
    //printf("Sale funcion principal - AGREGAR\n");

    if (listPtr == NULL)
    {
        //printf("Entra cuando la lista es nula\n");
        return newStudentPtr;
    }else if (listPtr->siguiente == NULL)
    {
        listPtr->siguiente = newStudentPtr;
        newStudentPtr->siguiente = NULL;
        newStudentPtr = NULL;
        return listPtr;
    }
        //printf("Entra cuando la lista no es nula\n");
        Estudiante_t *ulitmoElemPtr = ultimoElementoLista(listPtr);
        ulitmoElemPtr->siguiente = newStudentPtr; // Enlazamos al final de la lista.
        //printf("Pasa asignacion de puntero newStudent\n");
        newStudentPtr->siguiente = NULL;
        newStudentPtr = NULL;
        ulitmoElemPtr = NULL;
        //printf("Pasa liberacion de ultimoElemPtr\n");
        return listPtr;
}

void displayList(Estudiante_t *listPtr, SortBy_t filtro) { // Toma como entrada el puntero al primer elemento de la lista.
    printf("Entra funcion principal - DISPLAY\n");
    sortList(listPtr, filtro); // Ordena la lista segun el filtro seleccionado
    if (filtro != SIN_FILTRO)
    {   
        printf("Llega previo a entrar a makeCircular\n");
        makeListCircular(listPtr); // Cierra la lista haciendola circular
        printf("Sale de makeCircular\n");
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
        actualPtr=NULL;
        printf("Sale funcion principal - BORRAR\n");
    return;
}