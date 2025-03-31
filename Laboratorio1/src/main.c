#include <stdio.h>
#include "my_lib_1.h"
#include "my_lib_2.h"


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
Estudiante_t *listFirst; // Apunta al primer elemento de la lista. Una vez que se crea el primer elemento, no se va a mover de alli.

int main() {
    listFirst = inicializarEstudiante(); // INICIALIZAR LISTA
    accesoMenu(); // Entramos al MENU


    my_lib_1_function();
    my_lib_2_function();
    return 0;
}
