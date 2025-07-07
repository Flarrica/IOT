
#include<iostream>
#include<string>
 using namespace std;
void trina_copy() {
    char origen[] = "Hola, mundo!";
    char destino[50];  

  
    char *ptr_origen = origen;
    char *ptr_destino = destino;

    while (*ptr_origen != '\0') {
        *ptr_destino = *ptr_origen;
        ptr_origen++;
        ptr_destino++;
    }
    *ptr_destino = '\0';  
    cout<< "\n--------------------------------------------------------"<<endl;
    cout<< "El string copiado es:"<<destino<<endl;
    cout<< "\n--------------------------------------------------------"<<endl;
    
}

int main() {
    trina_copy();  
    return 0;
}
