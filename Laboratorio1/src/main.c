#include <stdio.h>
#include <stdbool.h>
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

int main() {


        //Prueba de la primera parte Libreria 1_2

        // 1. init_lab
        init_lab();

        // 2. eq_solver
        coeff_t coef = {1, -3, 2};  // x^2 - 3x + 2 = 0
        eq_solver(&coef);
    
        // 3. bin2dec
        int32_t binario = 1101;
        bin2dec(binario, false);
        bin2dec(1010, false);  // 10 en decimal (sin signo)
        bin2dec(1101, true);   // -3 en decimal (con signo) 
    
        // 4. print_reverse_array
        int arr[] = {1, 2, 3, 4, 5};
        print_reverse_array(arr, sizeof(int), 5);
    
        // 5. max_index y min_index
        max_index(arr, sizeof(int), 5);
        min_index(arr, sizeof(int), 5);
    
        // 6. crear_matriz + matrix_sub
        // matriz de 2*2
        matriz_t A = crear_matriz(2, 2);
        matriz_t B = crear_matriz(2, 2);
    
        A.datos[0][0] = 5; A.datos[0][1] = 6;
        A.datos[1][0] = 7; A.datos[1][1] = 8;
    
        B.datos[0][0] = 1; B.datos[0][1] = 2;
        B.datos[1][0] = 3; B.datos[1][1] = 4;

        matriz_t R = matrix_sub(A, B);
        printf("Resultado de la resta de matrices:\n");
        for (size_t i = 0; i < R.filas; i++) {
            for (size_t j = 0; j < R.columnas; j++) {
                printf("%d ", R.datos[i][j]);
            }
            printf("\n");
        }
    
        // Liberar memoria de matrices
        for (size_t i = 0; i < A.filas; i++) {
            free(A.datos[i]);
            free(B.datos[i]);
            free(R.datos[i]);
        }
        free(A.datos);
        free(B.datos);
        free(R.datos);
    
        // 7. consonantes y vocales
        char texto1[] = "Hola Mundo";
        consonantes(texto1);
        vocales(texto1);
    
        // 8. reverse_string
        char mensaje[] = "Hola Mundo";
        printf("Antes de invertir: %s\n", mensaje);
        reverse_string(mensaje);
        printf("Después de invertir: %s\n", mensaje);
    
        // 9. swap
        int x = 10, y = 20;
        printf("Antes del swap: x = %d, y = %d\n", x, y);
        if (swap(&x, &y, sizeof(int)) == 0) {
            printf("Después del swap: x = %d, y = %d\n", x, y);
        } else {
            printf("Error en swap\n");
        }
    
        // 10. string_length
        char cadena[] = "Lenguaje C";
        int32_t largo = string_length(cadena);
        printf("La cadena de caracteres era: %s\n", cadena);
        printf("Longitud del string: %d\n", largo);
        
    // Final de la prueba de libreria 1_2

    //Prueba de la primera parte Libreria 1_1

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

    // Final de la prueba de libreria 1_1


    //Prueba de la primera parte Parte 2

    // Agregamos estudiantes con datos correctos

    Estudiante_t *listaEstudiantes = NULL;
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Catalina", "Viera", "89012345", CUARTO, 82);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Alan", "Villalba", "90123456", PRIMERO, 79);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Milagros", "Zeballos", "91234567", QUINTO, 94);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Kevin", "Zunino", "92345678", SEXTO, 88);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Agustina", "Abreu", "93456789", TERCERO, 3);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Dylan", "Alonso", "94567890", SEGUNDO, 92);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Kiara", "Andrade", "95678901", CUARTO, 56);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Ian", "Arce", "96789012", PRIMERO, 80);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Ashley", "Arismendi", "97890123", QUINTO, 93);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Axel", "Barrios", "98901234", SEXTO, 86);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Zoe", "Bentancur", "99012345", TERCERO, 79);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Thiago", "Burgos", "40123456", SEGUNDO, 98);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Maite", "Cabrera", "41234567", CUARTO, 83);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Santino", "Caceres", "42345678", PRIMERO, 89);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Benicio", "Campos", "44567890", SEXTO, 87);
    listaEstudiantes = printfList(listaEstudiantes, SIN_FILTRO);
    listaEstudiantes = printfList(listaEstudiantes, APELLIDO);
    listaEstudiantes = printfList(listaEstudiantes, NOMBRE);
    listaEstudiantes = agregarNuevoEstudiante(listaEstudiantes, "Diego", "Massaferro", "43748445", QUINTO, 79);
    listaEstudiantes = printfList(listaEstudiantes, CI);
    deleteStudentFromList(listaEstudiantes, "43748445");
    listaEstudiantes = printfList(listaEstudiantes, CI);

    return 0;
}
