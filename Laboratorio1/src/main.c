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
// VERIFICAR SI ESTA BIEN DECLARADA EN EL MAIN O DEBE IR EN LA LIBRERIA
// *listStudent; // Apunta al primer elemento de la lista. Una vez que se crea el primer elemento, no se va a mover de alli.
//Estudiante_t *listResult; // Se crea lista vacia para almacenar lso resultados de busquedas.
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
        char texto[] = "Hola Mundo";
        consonantes(texto);
        vocales(texto);
    
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

    //listStudent = inicializarEstudiante(listStudent); // INICIALIZAR LISTA DE ESTUDIANTES
    //listResult = inicializarEstudiante(listResult); // INICIALIZAR LISTA DE RESULTADOS DE BUSQUEDA
    //accesoMenu(listStudent, listResult); // Entramos al MENU


    return 0;
}
