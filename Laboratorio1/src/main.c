#include <stdio.h>
#include "Global.h"


/* 
Para compilar, ingresar los siguientes comandos en "cmd"


cd Desktop\IoT\IOT\Laboratorio1
rmdir /s /q build
del CMakeCache.txt
build.cmd compile
build.cmd run



*/
// VERIFICAR SI ESTA BIEN DECLARADA EN EL MAIN O DEBE IR EN LA LIBRERIA
// *listStudent; // Apunta al primer elemento de la lista. Una vez que se crea el primer elemento, no se va a mover de alli.
Estudiante_t *listaEstudiantes = NULL;
int main() {
    // Agregamos estudiantes con datos correctos
    agregarNuevoEstudiante(listaEstudiantes, "Catalina", "Viera", "89012345", CUARTO, 82);
    agregarNuevoEstudiante(listaEstudiantes, "Alan", "Villalba", "90123456", PRIMERO, 79);
    agregarNuevoEstudiante(listaEstudiantes, "Milagros", "Zeballos", "91234567", QUINTO, 94);
    agregarNuevoEstudiante(listaEstudiantes, "Kevin", "Zunino", "92345678", SEXTO, 88);
    agregarNuevoEstudiante(listaEstudiantes, "Agustina", "Abreu", "93456789", TERCERO, 78);
    agregarNuevoEstudiante(listaEstudiantes, "Dylan", "Alonso", "94567890", SEGUNDO, 92);
    agregarNuevoEstudiante(listaEstudiantes, "Kiara", "Andrade", "95678901", CUARTO, 85);
    agregarNuevoEstudiante(listaEstudiantes, "Ian", "Arce", "96789012", PRIMERO, 80);
    agregarNuevoEstudiante(listaEstudiantes, "Ashley", "Arismendi", "97890123", QUINTO, 93);
    agregarNuevoEstudiante(listaEstudiantes, "Axel", "Barrios", "98901234", SEXTO, 86);
    agregarNuevoEstudiante(listaEstudiantes, "Zoe", "Bentancur", "99012345", TERCERO, 79);
    agregarNuevoEstudiante(listaEstudiantes, "Thiago", "Burgos", "40123456", SEGUNDO, 91);
    agregarNuevoEstudiante(listaEstudiantes, "Maite", "Cabrera", "41234567", CUARTO, 83);
    agregarNuevoEstudiante(listaEstudiantes, "Santino", "Caceres", "42345678", PRIMERO, 81);
    agregarNuevoEstudiante(listaEstudiantes, "Benicio", "Campos", "44567890", SEXTO, 87);
    listaEstudiantes = printfList(listaEstudiantes, SIN_FILTRO);
    listaEstudiantes = printfList(listaEstudiantes, APELLIDO);
    // Entre estas dos lineas pasa algo con los punteros que hace que no funcione
    listaEstudiantes = printfList(listaEstudiantes, NOMBRE);
    agregarNuevoEstudiante(listaEstudiantes, "Diego", "Massaferro", "43748445", QUINTO, 79);
    listaEstudiantes = printfList(listaEstudiantes, CI);
    deleteStudentFromList(listaEstudiantes, "43748445");
    listaEstudiantes = printfList(listaEstudiantes, CI);
    return 0;
}
