#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>

// weas del popen
#include <memory>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <array>

using namespace std;

/* definicion de limites */
#define MAX_PING 5
static const int MAX_THREAD = 5;

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
void createPing(ping ping, string cant)
{
    // creacion del comando de
    string command = "ping " + ping.ip + " -c " + cant + " -q";

    /*
        la siguiente seccion se encarga de generar los pings
        y recuperar el output mediante el uso de popen()
        el output del comando ping se guarda en result
    */
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    // cout<< command;
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }

    if (result == "")
    {
        throw std::runtime_error("El ping fallo");
    }
    // cout<< result;

    // busca las posiciones de los valores relevantes
    int transPos = result.find(" packets transmitted") - 1;
    int recPos = result.find(" received") - 1;
    int lossPos = result.find(" packet loss") - 2;

    // copia y guarda el valor de los paquetes transmitidos
    char aux[1];
    result.copy(aux, 1, transPos);
    // cout<<aux<<" \n";
    ping.pSend = atoi(aux);
    //*aux= '\0';

    // copia y guarda el valor de los paquetes recividos
    result.copy(aux, 1, recPos);
    ping.pRec = atoi(aux);

    // copia y guarda el valor de los paquetes perdidos
    result.copy(aux, 1, lossPos);
    ping.pLoss = atoi(aux);

    if (ping.pRec >= 1)
    {
        ping.state = "UP";
    }
    else
    {
        ping.state = "DOWN";
    }

    // imprime el resultado completo
    if (ping.ip.length() >= 15)
    {
        cout << ping.ip << " \t" << ping.pSend << " \t" << ping.pRec << " \t" << ping.pLoss << " \t" << ping.state << "\n";
    }
    else
    {
        cout << ping.ip << " \t\t" << ping.pSend << " \t" << ping.pRec << " \t" << ping.pLoss << " \t" << ping.state << "\n";
    }
}

int main(int argc, char *argv[])
{
    // comprueba que se hayan ingresado la cant de arg necesarios
    if (argc < 2)
    {
        cout << "No se ingresaron los argumentos suficientes (2) \n";
        return 1;
    }
    // convierte el argumento 2 a integer
    string cantPackages = argv[2];
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
            // cout<< ip<<" \n";
            pings[count].ip = ip;
            count++;
        }
    }

    /* Manejo de hilos */
    // creacion de hilos
    thread threads[MAX_THREAD];

    // print de confirmacion que no esta explotando
    // la idea es que en createPing se imprima formateado
    cout << "IP \t\t   Trans. \tRec. \t Perd. \t Estado \n";
    cout << "--------------------------------------------------------\n";

    // inicializacion de hilos con la funcion que ejecuta ping
    for (int i = 0; i < MAX_THREAD; i++)
    {
        threads[i] = std::thread(createPing, pings[i], cantPackages);
    }

    // sincronzando los hilos al main
    for (int i = 0; i < 5; i++)
    {
        threads[i].join();
    }

    // cout << "\n";

    // for (int i = 0; i < MAX_THREAD; i++)
    // {
    //     if (pings[i].ip.length() >= 15)
    //     {
    //         cout << pings[i].ip << " \t" << pings[i].pSend << " \t" << pings[i].pRec << " \t" << pings[i].pLoss << " \t" << pings[i].state << "\n";
    //     }
    //     else
    //     {
    //         cout << pings[i].ip << " \t\t" << pings[i].pSend << " \t" << pings[i].pRec << " \t" << pings[i].pLoss << " \t" << pings[i].state << "\n";
    //     }
    // }

    return 0;
}