#include <iostream>
#include <fstream>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>
#include <memory>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <array>

using namespace std;

/* definicion de limites */
static const int MAX_THREAD = 5;

/*
    Estructura de datos que almacena
    los reportes de los pings
*/
class Ping
{
public:
    string ip;
    int pSend;
    int pRec;
    int pLoss;
    string state;
};

/*
    funcion que crea los pings
*/
void createPing(Ping *ping, string cant)
{
    // creacion del comando de
    string command = "ping " + ping->ip + " -c " + cant + " -q";

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
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }

    if (result == "")
    {
        throw std::runtime_error("El ping fallo");
    }
    /*
    PING 62.233.190.103 (62.233.190.103) 56(84) bytes of data.

    --- 62.233.190.103 ping statistics ---
    5 packets transmitted, 0 received, 100% packet loss, time 4079ms

    */

    // la siguiente seccion se encarga de buscar el valor de los paquetes recibidos
    char * aux;
    char * cantRec;
    char *output = new char[result.length()+1];
    strcpy(output,result.c_str());
    aux=strtok(output," ");
    do
    {
        aux=strtok(NULL," ");
        //cout<<aux<<endl;
        if(strcmp(aux,"received,")==0){
            //cout<<"cantRec: "<<cantRec<<endl;
            ping->pRec=atoi(cantRec);
        }else{
            
            cantRec=aux;
        }
    }while (strcmp(aux,"received,")!=0);
    // guarda la cant de paquetes enviados
    ping->pSend=atoi(cant.c_str());
    // calcula y guarda la cantidad de paquetes perdidos
    ping->pLoss=ping->pSend-ping->pRec;

    if (ping->pRec >= 1)
    {
        ping->state = "UP";
    }
    else
    {
        ping->state = "DOWN";
    }

    // imprime el resultado completo
    if (ping->ip.length() >= 15)
    {
        cout << ping->ip << " \t" << ping->pSend << " \t" << ping->pRec << " \t" << ping->pLoss << " \t" << ping->state << "\n";
    }
    else
    {
        cout << ping->ip << " \t\t" << ping->pSend << " \t" << ping->pRec << " \t" << ping->pLoss << " \t" << ping->state << "\n";
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
    int count = 0;
    // si el archivo esta abierto
    if (ipFile.is_open())
    {
        // recorrer el archivo guardando las ip
        while (ipFile.good())
        {
            // lee y guarda hasta el primer salto de linea
            ipFile >> ip;
            count++;
        }
    }

    // reinicio del archivo para guardar las ips en el array de pings
    // ipFile.clear();
    // ipFile.seekg(0,std::ios::beg);
    ipFile.close();
    ipFile.open(argv[1]);

    // creacion de array de pings
    Ping pings[count];
    count = 0;
    if (ipFile.is_open())
    {
        while (ipFile.good())
        {
            ipFile >> ip;
            pings[count].ip = ip;
            count++;
        }
    }
    /* Manejo de hilos */
    cout << "ola" << endl;
    // creacion de hilos
    thread threads[MAX_THREAD];

    // print de confirmacion que no esta explotando
    // la idea es que en createPing se imprima formateado
    cout << "IP \t\t   Trans. \tRec. \t Perd. \t Estado \n";
    cout << "--------------------------------------------------------\n";

    // inicializacion de hilos con la funcion que ejecuta ping
    int restantes = count;
    while (restantes > 0)
    {
        for (int i = 0; i < MAX_THREAD; i++)
        {
            threads[i] = std::thread(&createPing, &pings[i], cantPackages);
            restantes--;
        }

        // sincronzando los hilos al main
        for (int i = 0; i < MAX_THREAD; i++)
        {
            threads[i].join();
        }
    }

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