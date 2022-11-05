#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

using namespace std;

/* definicion de limites */
#define MAX_PING 5
#define MAX_THREAD 5;

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

int main(int argc, char *argv[])
{
    // comprueba que se hayan ingresado la cant de arg necesarios
    if (argc < 2)
    {
        cout << "No se ingresaron los argumentos suficientes (2) \n";
        return 1;
    }

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
}