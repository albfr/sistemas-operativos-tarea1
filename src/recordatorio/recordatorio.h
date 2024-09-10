#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>

void imprimirUsoCorrectoRecordatorio()
{
    std::cout << "Uso: set [comando] [ajustes]\n";
    std::cout << "Permite definir un recordatorio\n";
    std::cout << "A continuación una lista con los comandos disponibles y sus descripciones\n";

    std::cout << "    ayuda                        ";
    std::cout << "despliega información sobre uso básico del comando\n";

    std::cout << "    recordatorio [tiempo][nombre]     ";
    std::cout << "define un recordatorio\n";
    std::cout << "\n";
}

int manejarAyudaRecordatorio(const std::vector<std::string> &argv)
{
    imprimirUsoCorrectoRecordatorio();
    return 0;
}

void mostrarRecordatorio(const std::string &mensaje, int tiempoSegundos)
{
    std::cout << "Esperando " << tiempoSegundos << " segundos para mostrar el recordatorio...\n";
    std::this_thread::sleep_for(std::chrono::seconds(tiempoSegundos));
    std::cout << "\nRecordatorio: " << mensaje << std::endl;
    std::cout << std::flush;
}

int manejarRecordatorio(const std::vector<std::string> &argv)
{
    /*
        argv[2]:        tiempo en segundos
        argv[3...n]:    mensaje
    */

    if (argv.size() < 3)
    {
        std::cerr << "Error: falta tiempo o mensaje\n";
        return 1;
    }

    // Extraer el tiempo de argv[2]
    int tiempoSegundos;
    try
    {
        tiempoSegundos = std::stoi(argv[2]);
    }
    catch (const std::invalid_argument &)
    {
        std::cerr << "Error: tiempo inválido\n";
        return 1;
    }

    // Extraer el mensaje de argv[3...n]
    std::string mensaje;
    for (size_t i = 3; i < argv.size(); ++i)
    {
        if (i > 3)
        {
            mensaje += " ";
        }
        mensaje += argv[i];
    }

    // Crear un hilo para manejar el temporizador y el recordatorio
    std::thread recordatorioThread(mostrarRecordatorio, mensaje, tiempoSegundos);
    recordatorioThread.detach(); // Desacoplar el hilo para que siga ejecutándose en segundo plano

    // Note que detach es importante para que el hilo no bloquee el hilo principal, y, por lo tanto,
    // la shell pueda seguir utilizandose mientras se espera el tiempo del recordatorio.

    // Mensaje para indicar que el recordatorio se ha programado
    std::cout << "Recordatorio programado.\n";
    std::cout << std::flush;

    return 0;
}
