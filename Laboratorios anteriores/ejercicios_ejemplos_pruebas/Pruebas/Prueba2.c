#pragma warning(disable:4996)

#include <stdio.h>

int main()
{
    int x;

    // Solicitar al usuario que ingrese un valor para x
    printf("Ingresa un valor para x: ");
    scanf("%d", &x);

    if (x > 5)
    {
        printf("x es mayor a 5\n");
    }
    else
    {
        printf("x es menor o igual a 5\n");
    }

    return 0;
}
