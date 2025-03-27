


/*
    Realizar un programa que reste dos matrices
----------------------------------------------------
*/
#include<iostream>
    using namespace std;
int main() {

    float matriz1 [3][3]= {{1,2,3},
                        {4,5,6},{
                         7,8,9} };
                         
    float matriz2 [3][3]= {{1,2,3},
                        {4,5,6},{
                         7,8,9} };

    float matrizresultado[3][3];
    
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            matrizresultado[i][j]=matriz1[i][j] - matriz2[i][j];
            
        }
        
    }
    
    cout<< "El resultado de la resta es: "<< endl;
    
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            cout << matrizresultado[i][j]<< " ";
        }
        
        cout << endl;
        
    }

    return 0;
}