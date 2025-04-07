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
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Catalina", "Viera", "89012345", CUARTO, 82);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Alan", "Villalba", "90123456", PRIMERO, 79);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Milagros", "Zeballos", "91234567", QUINTO, 94);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Kevin", "Zunino", "92345678", SEXTO, 88);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Agustina", "Abreu", "93456789", TERCERO, 78);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Dylan", "Alonso", "94567890", SEGUNDO, 92);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Kiara", "Andrade", "95678901", CUARTO, 85);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Ian", "Arce", "96789012", PRIMERO, 80);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Ashley", "Arismendi", "97890123", QUINTO, 93);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Axel", "Barrios", "98901234", SEXTO, 86);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Zoe", "Bentancur", "99012345", TERCERO, 79);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Thiago", "Burgos", "40123456", SEGUNDO, 91);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Maite", "Cabrera", "41234567", CUARTO, 83);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Santino", "Cáceres", "42345678", PRIMERO, 81);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Benicio", "Campos", "44567890", SEXTO, 87);
    displayList(listaEstudiantes, NOMBRE);
    displayList(listaEstudiantes, APELLIDO);
    displayList(listaEstudiantes, CI);
    deleteStudentFromList(listaEstudiantes, "41234567");
    displayList(listaEstudiantes, APELLIDO);
    return 0;
}
