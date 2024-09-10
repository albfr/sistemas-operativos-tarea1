#include "subcomandos.h"

#ifndef __IOSTREAM_H
#include <iostream>
#endif
#ifndef __STRING_H
#include <string>
#endif
#ifndef __VECTOR_H
#include <vector>
#endif

std::vector<std::string> comandos_disponibles = {"ayuda", "crear", "mostrar", "eliminar", "buscar", "borrar", "ejecutar", "cargar", "guardar"};

// retorna indice del comando en comandos_disponibles, retorna -1 si no existe
int determinarComando(const std::string &cmd) {
    for (int i = 0; i < comandos_disponibles.size(); i++) {
        if (cmd.compare(comandos_disponibles[i]) == 0) {
            return i;
        }
    }
    return -1;
}

//int favs_process(int argc, char *argv[]) {
int main(int argc, char *argv[]) {
    std::vector<std::string> sargv;
    for (int i = 0; i < argc; i++)
        sargv.push_back(std::string(argv[i]));

    if (argc <= 1) {
        std::cerr << "Error: no hay comando\n";
        imprimirUsoCorrecto();
        exit(1);
    }

    int cmd = determinarComando(argv[1]);
    if (cmd == -1) {
        std::cerr << "Error: comando " << argv[1] << " no existe\n";
        std::cerr << "Usar \"favs ayuda\" para mas información\n";
        exit(1);
    }

    int error;
    switch (cmd) {
        case 0:
            error = manejarAyuda(sargv);
            break;
        case 1:
            error = manejarCrear(sargv);
            break;
        case 2:
            error = manejarMostrar(sargv);
            break;
        case 3:
            error = manejarEliminar(sargv);
            break;
        case 4:
            error = manejarBuscar(sargv);
            break;
        case 5:
            error = manejarBorrar(sargv);
            break;
        case 6:
            // error = manejarEjecutar(sargv);
            break;
        case 7:
            error = manejarCargar(sargv);
            break;
        case 8:
            error = manejarGuardar(sargv);
            break;
        default:
            error = 1;
            break;
    }
}