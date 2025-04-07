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
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "María", "González", "51234567", TERCERO, 90);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Juan", "Rodríguez", "52345678", SEGUNDO, 90);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Carlos", "López", "54567890", PRIMERO, 90);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Laura", "Pérez", "53456789", CUARTO, 90);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Carlos", "López", "54567890", PRIMERO, 90);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Ana", "Martínez", "55678901", QUINTO, 90);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Diego", "García", "56789012", SEXTO, 90);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Sofía", "Fernández", "57890123", TERCERO, 90);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Mateo", "Ruiz", "58901234", SEGUNDO, 90);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Valentina", "Díaz", "59012345", CUARTO, 82);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Nicolás", "Álvarez", "60123456", PRIMERO, 79);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Camila", "Ramírez", "61234567", QUINTO, 93);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Santiago", "Torres", "62345678", SEXTO, 86);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Isabella", "Gómez", "63456789", TERCERO, 77);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Sebastián", "Sosa", "64567890", SEGUNDO, 91);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Lucía", "Silva", "65678901", CUARTO, 83);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Andrés", "Vázquez", "66789012", PRIMERO, 81);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Martina", "Romero", "67890123", QUINTO, 94);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Agustín", "Suárez", "68901234", SEXTO, 87);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Emilia", "Méndez", "69012345", TERCERO, 76);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Joaquín", "Pereira", "70123456", SEGUNDO, 89);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Florencia", "Acosta", "71234567", CUARTO, 84);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Tomás", "Benítez", "72345678", PRIMERO, 82);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Josefina", "Herrera", "73456789", QUINTO, 96);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Federico", "Leal", "74567890", SEXTO, 88);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Victoria", "Núñez", "75678901", TERCERO, 78);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Máximo", "Olivera", "76789012", SEGUNDO, 92);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Renata", "Ortega", "77890123", CUARTO, 85);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Benjamín", "Paz", "78901234", PRIMERO, 80);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Abril", "Quiroga", "79012345", QUINTO, 93);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Thiago", "Ramos", "80123456", SEXTO, 86);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Paula", "Ríos", "81234567", TERCERO, 79);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Bruno", "Rocha", "82345678", SEGUNDO, 91);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Delfina", "Santana", "83456789", CUARTO, 83);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Lautaro", "Santos", "84567890", PRIMERO, 81);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Guillermina", "Sosa", "85678901", QUINTO, 95);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Ignacio", "Suárez", "86789012", SEXTO, 87);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Juliana", "Techera", "87890123", TERCERO, 77);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Facundo", "Toledo", "88901234", SEGUNDO, 90);
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
