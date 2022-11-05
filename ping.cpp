#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <thread>

using namespace std;

/* definicion de limites */
#define MAX_PING 5
static const int MAX_THREAD=5;

/*
    Estructura de datos que almacena
    los reportes de los pings
*/
struct ping
{
    string ip;
    int pSend;
    int pRec;
    int pLoss;
    string state;
} pings[MAX_PING];

/*
    funcion que crea los pings
*/
void createPing(ping ping){
    // aqui haria un ping por consola, si supiera como
}

int main(int argc, char *argv[])
{
    // comprueba que se hayan ingresado la cant de arg necesarios
    if (argc < 2)
    {
        cout << "No se ingresaron los argumentos suficientes (2) \n";
        return 1;
    }
    /* manejo de archivos */ 

    // abrir el archivo de ips usando el argumento[1]
    std::ifstream ipFile;
    ipFile.open(argv[1]);

    // manejo de strings
    std::string ip;

    // si el archivo esta abierto 
    if (ipFile.is_open())
    {
        // recorrer el archivo guardando las ip
        int count = 0;
        while (ipFile.good())
        {
            // lee y guarda hasta el primer salto de linea
            ipFile >> ip;
            pings[count].ip = ip;
            count++;
        }
    }

    /* Manejo de hilos */
    // creacion de hilos
    thread threads[MAX_THREAD];

    // inicializacion de hilos con la funcion que ejecuta ping
    for(int i=0;i<MAX_THREAD;i++){
        threads[i]=thread(createPing, pings[i]);
    }

    cout<< "Inicializando...\n";

    // sincronzando los hilos al main
    for (int i = 0; i < MAX_THREAD; i++)
    {
        threads[i].join();
    }
    

    return 0;
}