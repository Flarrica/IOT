#include <stdio.h>
#include <math.h>

int main()
{
    int binario = 0b1010;
    int resultado = 0;
    int contador = 0;
    
    while (binario > 0)
    {
        if ((binario & 1) == 1){

            int exponente = pow(2,contador);
            contador++;
            resultado += exponente;
            binario = binario >> 1;
        }
        else{
            binario = binario >> 1;
            contador++;
        }
    }
    
    printf("El numero binario pasado a decimal es : %d \n", resultado);
    return 0;
}
