#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#define RUTA_FAVS "ruta.txt"

std::vector<std::vector<std::string>> favs_cmds;

void imprimirUsoCorrecto() {
    std::cout << "Uso: favs [comando] [opciones]\n";
    std::cout << "Permite guardar lista de comandos favoritos\n";
    std::cout << "A continuación una lista con los comandos disponibles y sus descripciones\n";

    std::cout << "    ayuda                    ";
    std::cout << "despliega información sobre uso básico del comando\n";

    std::cout << "    crear [ruta]/archivo     ";
    std::cout << "crea archivo donde se almacenan los comandos favoritos dada la ruta y nombre de archivo\n";
    std::cout << "                             ";
    std::cout << "el archivo no puede ser " << RUTA_FAVS << " y puede haber únicamente un archivo con favoritos, al crear\n";
    std::cout << "                             ";
    std::cout << "uno nuevo se sobreescribirá el anterior\n";

    std::cout << "    mostrar                  ";
    std::cout << "despliega lista de comandos favoritos existente\n";

    std::cout << "    eliminar num[,num]...    ";
    std::cout << "elimina comandos asociados a los numeros entre las comas\n";

    std::cout << "    buscar str               ";
    std::cout << "busca comandos que contengan str como substring en la lista de favoritos\n";

    std::cout << "    borrar                   ";
    std::cout << "borra todos los comandos en la lista de favoritos\n";

    std::cout << "    ejecutar num             ";
    std::cout << "ejecuta el comando asociado al número num\n";

    std::cout << "    cargar                   ";
    std::cout << "lee y carga comandos de archivo de favoritos en la sesión actual\n";

    std::cout << "    guardar                  ";
    std::cout << "guarda comandos agregados en sesión actual, en archivo de guardado\n";
    std::cout << "\n";
}

int manejarAyuda(const std::vector<std::string> &argv) {
    imprimirUsoCorrecto();
    return 0;
}

int manejarCrear(const std::vector<std::string> &argv) {
    if (argv.size() < 3) {
        std::cerr << "Es necesario indicar un archivo\n";
        return 1;
    }

    if (argv[2].compare("ruta.txt") == 0) {
        std::cerr << "El archivo no puede llamarse ruta.txt\n";
        return 1;
    }

    std::ofstream ruta;
    ruta.open(RUTA_FAVS);
    ruta << argv[2];
    ruta.close();
    return 0;
}

int manejarMostrar(const std::vector<std::string> &argv) {

    return 0;
}

int manejarEliminar(const std::vector<std::string> &argv) {
    return 0;
}

int manejarBuscar(const std::vector<std::string> &argv) {return 0;}
int manejarBorrar(const std::vector<std::string> &argv) {return 0;}
int manejarEjecutar(const std::vector<std::string> &argv) {return 0;}
int manejarCargar(const std::vector<std::string> &argv) {return 0;}
int manejarGuardar(const std::vector<std::string> &argv) {return 0;}
