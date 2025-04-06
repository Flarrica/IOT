
/*
    2. Escribe un programa en C para encontrar los elementos máximo y mínimo en un array.
*/

#include<iostream>
    using namespace std;

    int main() {
        int numeros[10]; // reserva la cantidad de lugares.
        int n;
        do
        {
            cout<< "\nIngrese la cantidad de numeros a colocar (1-10): ";
            cin>> n;
        } while (n < 1|| n > 10);
        for (int i = 0; i < n; i++)
        {
            cout<< "\nIngrese el valor para la posicion "<< (i+1)<< ": ";
            cin >> numeros[i];
        }
        cout<< "\n Los valores de los vectores son: "<<endl;
        
        for (int i = 0; i < n; i++)
        {
            cout<< i+1<< " -> " << numeros[i]<<endl;   
        }
        cout<< "\n-------------------------------------------------------";

        int mayor = numeros[0];
        int pos= 0;
        for (int i = 0; i < n; i++)
        {
            if (numeros[i] > mayor )
            {
                mayor = numeros [i];
                pos = i;
            }
            
        }
        cout << "\nEl mayor valor del arreglo es: " << mayor << " La posicion es: "<<pos+1<< endl;
        cout<< "\n-------------------------------------------------------";

        int menor=numeros[0];
        for (int i = 0; i < n; i++)
        {
            if (numeros[i]<menor)
            {
               menor = numeros[i];
               pos= i;
            }
            
        }
        cout<< "\nEL menor valor del arreglo es: " << menor << " La posicion es: " << pos + 1<< endl;
        return 0;
    }