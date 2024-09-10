#include "recordatorio.h"

#ifndef __IOSTREAM_H
#include <iostream>
#endif
#ifndef __STRING_H
#include <string>
#endif
#ifndef __VECTOR_H
#include <vector>
#endif

std::vector<std::string> comandos_disponibles_set = {"ayuda", "recordatorio"};

// retorna indice del comando en comandos_disponibles, retorna -1 si no existe
int determinarComandoSet(const std::string &cmd) {
    for (int i = 0; i < comandos_disponibles_set.size(); i++) {
        if (cmd.compare(comandos_disponibles_set[i]) == 0) {
            return i;
        }
    }
    return -1;
}

void set_process(std::vector<std::string> &argv) {
    int argc = argv.size();
    if (argc <= 1) {
        // std::cerr << "Error: no hay comando\n";
        exit(1);
    }

    if (argc <= 1)
    {
        std::cerr << "Error: no hay comando\n";
        imprimirUsoCorrectoRecordatorio();
        exit(1);
    }

    int cmd = determinarComandoSet(argv[1]);
    if (cmd == -1)
    {
        std::cerr << "Error: comando " << argv[1] << " no existe\n";
        std::cerr << "Usar \"favs ayuda\" para mas información\n";
        exit(1);
    }

    int error;
    switch (cmd)
    {
    case 0:
        error = manejarAyudaRecordatorio(argv);
        break;
    case 1:
        error = manejarRecordatorio(argv);
        break;
    default:
        error = 1;
        break;
    }
}
