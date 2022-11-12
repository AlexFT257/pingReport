#include <iostream>
#include <fstream>
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
    // creacion del comando de ping
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
    // si el ping fallo por el motivo que sea, result deberia estar vacio
    if (result == "")
    {
        throw std::runtime_error("El ping fallo");
    }

    // la siguiente seccion se encarga de buscar el valor de los paquetes recibidos
    /*
        la funcion strtok(NULL," ") recorre el output del comando buscando los espacios,
        la idea es buscar la palabra received en output ya que el el output sigue el siguiente formato
        5 packets transmitted, 5 received, 0% packet loss, time 4006ms
        entonces al buscar por espacios cuando encuentre el string de "received," el valor anterior sera
        la cantidad de paquetes.
    */
    char *aux;
    char *cantRec;

    // transformacion de String a char * para uso de funcion strtok()
    char *output = new char[result.length() + 1];

    /*
        por restricciones de tipos de datos de strtok es necesario copiar los valores de result en output
        ya que strtok solo admite char * , pero result es un string asi que primero hay que copiar
        los contenidos de result en el output, pero como result es un string se necesita usar result.c_str()
    */
    strcpy(output, result.c_str());
    aux = strtok(output, " ");
    do
    {
        // strtok(NULL," ") avanza y guarda los caracteres del output
        aux = strtok(NULL, " ");
        // comprueba que si nos encontramos los paquetes recibidos
        if (strcmp(aux, "received,") == 0)
        {
            // en caso de que estemos ante los paquetes recibidos guarda el valor
            ping->pRec = atoi(cantRec);
        }
        else
        {
            // guarda el valor de strtok por si el siguiente es strtok es "received,"
            cantRec = aux;
        }
    } while (strcmp(aux, "received,") != 0);

    // guarda la cant de paquetes enviados
    ping->pSend = atoi(cant.c_str());

    // calcula y guarda la cantidad de paquetes perdidos
    ping->pLoss = ping->pSend - ping->pRec;

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
    string nombreArchivo = argv[1];
    if (nombreArchivo.find(".txt") == std::string::npos)
    {
        cout << "el archivo no es un archivo valido (.txt)" << endl;
        return 1;
    }

    try
    {
        int numero = stoi(argv[2]);
    }
    catch (std::exception const & e)
    {
        cout << "No se ingreso un numero valido (numeros naturales)" << '\n';
        return 1;
    }

    // convierte el argumento en string
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
    // creacion de hilos
    thread threads[count];

    // la idea es que en createPing se imprima formateado
    cout << "IP \t\t   Trans. \tRec. \t Perd. \t Estado \n";
    cout << "--------------------------------------------------------\n";

    // inicializacion de hilos con la funcion que ejecuta ping
    // int restantes = count;
    for (int i = 0; i < count; i++)
    {
        threads[i] = std::thread(&createPing, &pings[i], cantPackages);
        // restantes--;
    }

    // sincronzando los hilos al main
    for (int i = 0; i < count; i++)
    {
        threads[i].join();
    }

    return 0;
}