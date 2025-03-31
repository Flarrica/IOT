#include <stdio.h>
#include <math.h>

void Contador(char *puntero,int *vocal,int *consonante);

int main()
{
    // variables
    char texto[]="QWERTYUIOPASDFGHJKLÑZXCVBNMqwertyuiopasdfghjklñzxcvbnm";
    int vocales = 0;
    int consonantes = 0;

    // función
    Contador(texto,&vocales,&consonantes);

    //Mostrar si funcionó
    printf("La cantidad de vocales es de %d y la cantidad de consonantes es de %d. \n",vocales,consonantes);

    return 0;
}

void Contador(char *puntero,int *vocal,int *consonante){

    while (*puntero != '\0')
    {
        
        if ((*puntero > 64 && *puntero < 91) || (*puntero > 96 && *puntero < 123))
        //if ((*puntero >= 'A' && *puntero <= 'Z') || (*puntero >= 'a' && *puntero <= 'z'))
        {
            switch (*puntero)
            {
            case 'A':
                (*vocal)++;
                break;
            case 'a':
                (*vocal)++;
                break;
            case 'E':
                (*vocal)++;
                break;
            case 'e':
                (*vocal)++;
                break;
            case 'I':
                (*vocal)++;
                break;
            case 'i':
                (*vocal)++;
                break;
            case 'O':
                (*vocal)++;
                break;
                case 'o':
                (*vocal)++;
                break;
            case 'U':
                (*vocal)++;
                break;
            case 'u':
                (*vocal)++;
                break;   
            default:
                (*consonante)++;
                break;
            }
        }
        puntero++;     
    }
    

}