
#include <iostream>
    using namespace std;
// Se crea una funcion que cuenta palabras. Primero se verifica que el tring no sea vacio
int string_words(char *string) {
    if (!string || *string == '\0') return 0; 

    int count = 0;
    bool inWord = false;


    /* con un puntero recorremos los lugares de memoria si. con una variable bouleana cunado pasa de false a true 
    el count aumenta su valor porque comienza una nueva palabra*/
    while (*string) 
    {
        if (*string == ' ' || *string == '\t' || *string == '\n') {
            inWord = false;
        } else if (!inWord) {
            inWord = true;
            count++;
        }
        string++;
    }

    return count;
}

int main() {
    char texto[] = "Este es un ejemplo para contar las palabras";
     cout<< "\n--------------------------------------------------------"<<endl;
     cout << "Número de palabras: " << string_words(texto) <<endl;
     cout<< "\n--------------------------------------------------------"<<endl;
    return 0;
}
