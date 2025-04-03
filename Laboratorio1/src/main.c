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
Estudiante_t *listStudent;
int main() {
    listStudent = inicializarEstudiante(listStudent); // INICIALIZAR LISTA DE ESTUDIANTES
    accesoMenu(listStudent); // Entramos al MENU

    my_lib_1_1_function();
    my_lib_1_2_function();
    my_lib_2_function();
    return 0;
}
