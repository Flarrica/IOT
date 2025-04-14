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
Estudiante_t *ultimoElementoLista(Estudiante_t *listPtr) {// BUSCAR ULTIMO EN LISTA
    //printf("Entra funcion auxiliar - BUSCAR_ULTIMO_LISTA\n");
    Estudiante_t *actualPtr = listPtr; //Declaro e inicializo un puntero auxiliar que apunta al inicio de la lista.
    while (actualPtr->siguiente != NULL) { // Esta condicion se da cuando se llega al elemento final de la lista.
        actualPtr = actualPtr->siguiente; // Se itera apuntando el puntero auxiliar a la siguiente posicion
    }
    //printf("Sale funcion auxiliar - BUSCAR_ULTIMO_LISTA\n");
    return actualPtr; //Devolvemos el puntero para que lo tome otra funcion
}
//-----------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------AUXILIARES de funcion prinicpal: PRINTF LISTA------------------------------------------------//
Estudiante_t *buscaMenorElemento(Estudiante_t *listPtr, SortBy_t filtro) {
    //printf("->buscaMenorElemento: Entra\n");
    if (listPtr == NULL) {
        return NULL;
    }
    char sortedName[FORMAT_NAME];
    char sortedLastname[FORMAT_LASTNAME];
    char sortedCI[FORMAT_CI];
    Estudiante_t *actualPtr = listPtr; //Checkeamos
    Estudiante_t *sortedElemPtr = listPtr;
    int orden = 0; // El orden asignado a dos elementos, comparando por el filtro indicado en la funcion
    if (filtro != SIN_FILTRO)// Entro aca si tengo filtro aplicado
    {
        switch (filtro) {
            case NOMBRE:
            strcpy(sortedName, listPtr->nombre); // Apuntamos al primer nombre de la lista
            while (actualPtr->siguiente != NULL) {
                orden = strcmp(sortedName, actualPtr->nombre);// Compara el primer nombre guardado con el segundo
                if (orden >0) // Caso: Primer nombre Bruno. Segundo nombre Alma. Me quedo con el segundo como el menor.
                {
                    sortedElemPtr = actualPtr; // Ahora apunta al siguiente elemento.
                    strcpy(sortedName, sortedElemPtr->nombre); // Copia el nombre en la variable string auxiliar para comparar.
                }
                actualPtr = actualPtr->siguiente;
            }
            orden = strcmp(sortedName, actualPtr->nombre);
            if (orden >0) // Caso: Primer nombre Bruno. Segundo nombre Alma. Me quedo con el segundo como el menor.
                {
                    sortedElemPtr = actualPtr; // Ahora apunta al siguiente elemento.
                    strcpy(sortedName, sortedElemPtr->nombre); // Copia el nombre en la variable string auxiliar para comparar.
                }
                break;
            case APELLIDO:
            strcpy(sortedLastname, listPtr->apellido); // Apuntamos al primer apellido de la lista
            while (actualPtr->siguiente != NULL) {
                orden = strcmp(sortedLastname, actualPtr->apellido);// Compara el primer apellido guardado con el segundo
                if (orden >0) // Caso: Primer apellido Bruno. Segundo apellido Alma. Me quedo con el segundo como el menor.
                {
                    sortedElemPtr = actualPtr; // Ahora apunta al siguiente elemento.
                    strcpy(sortedLastname, sortedElemPtr->apellido); // Copia el apellido en la variable string auxiliar para comparar.
                }
                actualPtr = actualPtr->siguiente;
            }
            orden = strcmp(sortedLastname, actualPtr->apellido);
            if (orden >0) // Caso: Primer apellido Bruno. Segundo apellido Alma. Me quedo con el segundo como el menor.
                {
                    sortedElemPtr = actualPtr; // Ahora apunta al siguiente elemento.
                    strcpy(sortedLastname, sortedElemPtr->apellido); // Copia el apellido en la variable string auxiliar para comparar.
                }
                break;
            case CI:
            strcpy(sortedCI, listPtr->CI); // Apuntamos al primer CI de la lista
            while (actualPtr->siguiente != NULL) {
                orden = strcmp(sortedCI, actualPtr->CI);// Compara el primer CI guardado con el segundo
                if (orden >0) // Caso: Primer CI Bruno. Segundo CI Alma. Me quedo con el segundo como el menor.
                {
                    sortedElemPtr = actualPtr; // Ahora apunta al siguiente elemento.
                    strcpy(sortedCI, sortedElemPtr->CI); // Copia el CI en la variable string auxiliar para comparar.
                }
                actualPtr = actualPtr->siguiente;
            }
            orden = strcmp(sortedCI, actualPtr->CI);
            if (orden >0) // Caso: Primer CI Bruno. Segundo CI Alma. Me quedo con el segundo como el menor.
                {
                    sortedElemPtr = actualPtr; // Ahora apunta al siguiente elemento.
                    strcpy(sortedCI, sortedElemPtr->CI); // Copia el CI en la variable string auxiliar para comparar.
                }
                break;
            default:
            break;
        }  // A la salida de esto tengo el puntero sortedElemPtr que apunta al valor mas chico de todos.
    }
    actualPtr = NULL;
    return sortedElemPtr;
}
Estudiante_t *armarListaOrdenada(Estudiante_t *listPtr, SortBy_t filtro) {
    if (listPtr == NULL)
    {
        return listPtr;
    }else if (listPtr->siguiente == NULL)
    {
        return listPtr;
    }
    Estudiante_t *previousPtr = listPtr; // Va a apuntar al anterior al elemento sorteado como el "menor" de la lista
    Estudiante_t *sortedElementPtr = listPtr; // Apunta al menor de la lista con respecto al orden ASCII de los strings, segun filtro.
    Estudiante_t *ultimoSortedList = NULL; // Apunta al ultimo de la nueva lista sorteada.
    Estudiante_t *sortedListPtr = NULL; // Apunta a la nueva lista ordenada. Guarda los resultados del filtrado y al final se borra.
    do
    {
        sortedElementPtr = buscaMenorElemento(listPtr, filtro);
        // ACA QUITAMOS EL ELEMENTO FILTRADO DE LA LISTA ORIGINAL
        if (sortedElementPtr == listPtr)
        {
            listPtr = listPtr->siguiente; // La lista original se rearma y sacamos el elemento sorteado.
            }else
            {
                while (previousPtr->siguiente != sortedElementPtr) //Busca pararse en el elemento anterior al actual
                {
                    previousPtr = previousPtr->siguiente;
                } // Tengo el puntero previous al elemento anterior al sorteado de la lista .
                if (sortedElementPtr->siguiente == NULL) // Caso en que el elemento filtrado es el ultimo de la lista
                {
                    previousPtr->siguiente = NULL;
                }else 
                {
                    previousPtr->siguiente = sortedElementPtr->siguiente; // Engancho lista, dejando afuera el elemento actual
                }
            }

        // AHORA COLOCAMOS EL ELEMENTO FILTRADO EN LA NUEVA LISTA
        if (sortedListPtr == NULL) // INICIALIZAR LA LISTA EN CASO QUE SEA VACIA
        {
            sortedListPtr = sortedElementPtr;
            sortedListPtr->siguiente = NULL;
            sortedElementPtr = NULL;
        }else{ // SINO, BUSCAR EL ULTIMO ELEMENTO PARA ENGANCHAR DESDE AHI
            ultimoSortedList = ultimoElementoLista(sortedListPtr); // Busco el ultimo elemento de la lista ordenada
            ultimoSortedList->siguiente = sortedElementPtr; // Armo la lista
            ultimoSortedList->siguiente->siguiente = NULL; // Apunto el final a null
            sortedElementPtr = NULL;
        }
        previousPtr = listPtr; //REINICIAMOS LA POSICION DEL PUNTERO PREVIUOUS
    } while (listPtr->siguiente != NULL);
    // Aca tengo que pasar el ultimo elemento que quedo suelto.
    ultimoSortedList = ultimoElementoLista(sortedListPtr);
    ultimoSortedList->siguiente = listPtr;
    ultimoSortedList->siguiente->siguiente = NULL; // Apunto el final a null
    ultimoSortedList = NULL;
    previousPtr = NULL;
    sortedElementPtr = NULL;
    return sortedListPtr;
}
void printListHeader() { // PRINTF - Encabezado de la lista
    printf("\n");
    printf("\n");
    printf("%*s|%*s|%*s|%*s|%*s\n",FORMAT_NAME, "NOMBRE", FORMAT_LASTNAME, "APELLIDO", FORMAT_CI, "CI", FORMAT_GRADE, "GRADO", FORMAT_PROMCALIF, "CALIFICACION");
    printf("---------------------------------------------------------------\n");
    return;
}
void printStudentRow(Estudiante_t *actualPtr) { // PRINTF - Fila de la lista
    printf("%*s|%*s|%*s|%5d|%12d\n", FORMAT_NAME, actualPtr->nombre, FORMAT_LASTNAME, actualPtr->apellido, FORMAT_CI, actualPtr->CI, actualPtr->grado, actualPtr->promCalif );
    return;
}
//--------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------AUXILIARES de funcion prinicpal: BORRAR ELEMENTO-------------------------------------------------//
void deleteStudentElement(Estudiante_t *actualPtr, Estudiante_t *listPtr) {// Borra estudiante seleccionado
    if (actualPtr == listPtr) { //Caso 1: El elemento se encuentra justo al inicio de la lista.
        listPtr = listPtr->siguiente; // Si listPtr->siguiente = NULL, queda la lista nula.
        free(actualPtr);
    }else{
        Estudiante_t *auxPtr = listPtr; // Apunto al principio de la lista para iterar
        //makeListCircular(listPtr); // Hago la lista circular para poder recorrerla y llegar al elemento anterior de actualPtr
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
void deleteStudentOptions(Estudiante_t *actualPtr, Estudiante_t *listPtr) {// Da opciones para borrar estudiante encontrado
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
//--------------------------------------------------------------------------------------------------------------------------------------------//

//FUNCIONES PRINCIPALES
Estudiante_t *agregarNuevoEstudiante(Estudiante_t *listPtr, char nombre[FORMAT_NAME], char apellido[FORMAT_LASTNAME], char CI[FORMAT_CI], Grado_t grado, int promCalif) { 
    //VALIDACION DE ERRORES
    CodigoError_t error = validacionNuevoEstudiante (nombre, apellido, CI, grado, promCalif);
    if (error != NO_ERROR) // SI HAY ERROR, IMPRIME EN PANTALLA CUAL ES Y ABORTA LA FUNCION. SI NO HAY ERROR< SIGUE.
    {
        printErrorNuevoEstudiante(error);
        return listPtr;
    }
    // CUERPO DE LA FUNCION
    Estudiante_t *newStudentPtr = malloc(sizeof(Estudiante_t)); //Reservamos memoria para un estudiante
    // Cargar datos por campo.
    strcpy(newStudentPtr->nombre, nombre);
    strcpy(newStudentPtr->apellido, apellido);
    strcpy(newStudentPtr->CI, CI);
    newStudentPtr->grado = grado;
    newStudentPtr->promCalif = (unsigned char)promCalif;
    newStudentPtr->siguiente = NULL;
    if (listPtr == NULL)
    {
        return newStudentPtr;
    }else if (listPtr->siguiente == NULL)
    {
        listPtr->siguiente = newStudentPtr;
        newStudentPtr->siguiente = NULL;
        newStudentPtr = NULL;
        return listPtr;
    }
        Estudiante_t *ulitmoElemPtr = ultimoElementoLista(listPtr);
        ulitmoElemPtr->siguiente = newStudentPtr; // Enlazamos al final de la lista.
        newStudentPtr->siguiente = NULL;
        newStudentPtr = NULL;
        ulitmoElemPtr = NULL;
        return listPtr;
}
Estudiante_t * printfList(Estudiante_t *listPtr, SortBy_t filtro) {
    Estudiante_t *actualPtr = listPtr; // Apunto a elemento actual
    if (actualPtr == NULL) { // Contemplar lista vacia y de un solo elemento.
        printf("La lista esta vacia. Desea agregar un nuevo estudiante?\n");
    }else if (actualPtr->siguiente == NULL) {
        printListHeader();
        printStudentRow(actualPtr);
    }else{
        printListHeader();
        switch (filtro)
        {
        case NOMBRE:
        listPtr = armarListaOrdenada(listPtr, NOMBRE);
            break;
        case APELLIDO:
        listPtr = armarListaOrdenada(listPtr, APELLIDO);
            break;
        case CI:
        listPtr = armarListaOrdenada(listPtr, CI);
            break;
        default:
            break;
        }
        // Apuntamos el puntero actual al primer elemento de la lista ordenada segun filtro
        actualPtr = listPtr;
        do
        {
            printStudentRow(actualPtr);
            actualPtr = actualPtr->siguiente; // Iteramos en la lista.
        } 
        while (actualPtr->siguiente != NULL);
        printStudentRow(actualPtr);
    }
    actualPtr=NULL;
    return listPtr;
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