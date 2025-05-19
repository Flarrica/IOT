#include <stdio.h>
#include <math.h>

int main()
{
    // defino las matrices
    int m1[3][3]={{2,5,3},{7,7,2},{2,6,7}};
    int m2[3][3]={{3,9,4},{9,4,3},{1,6,3}};
    int m3[3][3];

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            m3[i][j]=m1[i][j]-m2[i][j];
            printf("La resta de las matrices en la posicion %d, %d es: %d.\n", i, j, m3[i][j]);
        }
    }

    return 0;
}
