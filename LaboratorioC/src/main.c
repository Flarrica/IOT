#include <stdio.h>
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
    

    char texto[50];
    //Funcion cuenta palabras
    string_words(texto);
    /*-----------------------------------------------*/

    //Funcion para cambiar de may a min y al reves
    solicitarTexto();
    /*-----------------------------------------------*/

    // Funcion para copiar un string
    char source[50];   
    char destination[50]; 
    string_copy(source, destination); 
    /*-----------------------------------------------*/
    //Funcion para calcular dias entre dos fechas
    ejecutarCalculoDeDias();
    
    /*-----------------------------------------------*/
    // Funcion para encontrar una dub cadena
    solicitar_texto();  
    /*-----------------------------------------------*/
    //Multiplica dos nnumeros complejos
    procesarMultiplicacionCompleja();
    /*-----------------------------------------------*/
    //Funcion suma dos numeros complejos
    procesarSumaCompleja();
    return 0;
}
