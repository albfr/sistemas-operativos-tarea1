#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <fstream>
#include <unistd.h>

#define RUTA_FAVS "ruta.txt"

std::vector<std::vector<std::string>> favs_cmds;

void imprimirUsoCorrecto()
{
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

    // Guardar la ruta completa del archivo de favoritos en la carpeta donde se ejecuta el comando
    std::ofstream ruta;
    ruta.open(RUTA_FAVS);
    ruta << argv[2]; // ruta.txt --> [ruta/misFavoritos.txt]
    ruta.close();

    std::string rutaCompleta = argv[2];

    size_t pos = rutaCompleta.find_last_of("/");
    std::string directorio = rutaCompleta.substr(0, pos);

    std::string archivo = rutaCompleta.substr(pos + 1);

    if (pos != std::string::npos and chdir(directorio.c_str()) == -1) {
        std::cerr << "Error al cambiar de directorio\n";
        return 1;
    }
    if (pos == std::string::npos)
        pos = 0;

    // Crear el archivo de favoritos en el directorio especificado
    std::ofstream rutaArchivo;
    rutaArchivo.open(archivo);
    rutaArchivo.close();

    return 0;
}

int manejarMostrar(const std::vector<std::string> &argv) {

    // Abrir el archivo que contiene la ruta al archivo de favoritos
    std::ifstream archivoRuta(RUTA_FAVS);
    if (!archivoRuta)
    {
        std::cerr << "Error: no se pudo abrir el archivo de favoritos o no existe.\n";
        return 1;
    }

    // Leer la ruta del archivo de favoritos desde "ruta.txt"
    std::string rutaFavoritos;
    std::getline(archivoRuta, rutaFavoritos);
    archivoRuta.close();

    // Intentar abrir el archivo de favoritos usando la ruta obtenida
    std::ifstream archivoFavoritos(rutaFavoritos);
    if (!archivoFavoritos)
    {
        std::cerr << "Error: no se pudo abrir el archivo de favoritos en la ruta especificada (" << rutaFavoritos << ").\n";
        return 1;
    }

    // Verificar si el archivo de favoritos está vacío
    archivoFavoritos.seekg(0, std::ios::end); // Mover el puntero al final del archivo
    if (archivoFavoritos.tellg() == 0)
    {
        std::cout << "El archivo de favoritos está vacío. No hay comandos registrados.\n";
        archivoFavoritos.close();
        return 0;
    }

    archivoFavoritos.seekg(0, std::ios::beg); // Regresar al inicio del archivo
    std::cout << "Lista de comandos favoritos:\n";
    std::string linea;
    int indice = 1; // indice

    while (std::getline(archivoFavoritos, linea))
    {
        std::cout << indice << ". " << linea << "\n"; // Mostrar el índice y el comando
        indice++;
    }

    archivoFavoritos.close();

    return 0;
}

int manejarEliminar(const std::vector<std::string> &argv)
{
    /*
        Recibe un string que contiene los n números de los comandos a eliminar separados por comas:
        "n1,n2,n3,...,nn"
    */

    if (argv.size() < 3)
    {
        std::cerr << "Debe especificar los números de los comandos a eliminar.\n";
        return 1;
    }

    // Abrir el archivo que contiene la ruta al archivo de favoritos
    std::ifstream archivoRuta(RUTA_FAVS);
    if (!archivoRuta)
    {
        std::cerr << "Error: no se pudo abrir el archivo de ruta o no existe (" << RUTA_FAVS << ").\n";
        return 1;
    }

    // Leer la ruta del archivo de favoritos desde "ruta.txt"
    std::string rutaFavoritos;
    std::getline(archivoRuta, rutaFavoritos);
    archivoRuta.close();

    // Abrir el archivo de favoritos
    std::ifstream archivoFavoritos(rutaFavoritos);
    if (!archivoFavoritos)
    {
        std::cerr << "Error: no se pudo abrir el archivo de favoritos en la ruta especificada (" << rutaFavoritos << ").\n";
        return 1;
    }

    // Leer todos los comandos de favoritos en un vector
    std::vector<std::string> comandos;
    std::string linea;
    while (std::getline(archivoFavoritos, linea))
    {
        comandos.push_back(linea);
    }
    archivoFavoritos.close();

    // Parsear los números de comandos a eliminar desde argv[2]
    std::string numerosEliminar = argv[2];
    std::vector<int> indicesAEliminar;
    std::stringstream ss(numerosEliminar);
    std::string item;

    while (std::getline(ss, item, ','))
    {
        try
        {
            int index = std::stoi(item);
            if (index > 0 && index <= comandos.size())
            {
                indicesAEliminar.push_back(index - 1);
            }
            else
            {
                std::cerr << "Número fuera de rango: " << index << "\n";
            }
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "Argumento inválido: " << item << "\n";
            return 1;
        }
    }

    std::sort(indicesAEliminar.rbegin(), indicesAEliminar.rend());

    // Eliminar los comandos correspondientes
    for (int index : indicesAEliminar)
    {
        if (index >= 0 && index < comandos.size())
        {
            comandos.erase(comandos.begin() + index);
        }
    }

    // Reescribir el archivo de favoritos con los comandos restantes
    std::ofstream archivoFavoritosOut(rutaFavoritos);
    if (!archivoFavoritosOut)
    {
        std::cerr << "Error al abrir el archivo de favoritos para escritura.\n";
        return 1;
    }

    for (const auto &comando : comandos)
    {
        archivoFavoritosOut << comando << "\n";
    }

    archivoFavoritosOut.close();

    return 0;
}

int manejarBuscar(const std::vector<std::string> &argv)
{
    if (argv.size() < 3)
    {
        std::cerr << "Debe especificar el comando a buscar.\n";
        return 1;
    }

    std::string buscarCmd = argv[2];

    // Abrir el archivo que contiene la ruta al archivo de favoritos
    std::ifstream archivoRuta(RUTA_FAVS);
    if (!archivoRuta)
    {
        std::cerr << "Error: no se pudo abrir el archivo de ruta o no existe (" << RUTA_FAVS << ").\n";
        return 1;
    }

    // Leer la ruta del archivo de favoritos desde "ruta.txt"
    std::string rutaFavoritos;
    std::getline(archivoRuta, rutaFavoritos);
    archivoRuta.close();

    // Abrir el archivo de favoritos
    std::ifstream archivoFavoritos(rutaFavoritos);
    if (!archivoFavoritos)
    {
        std::cerr << "Error: no se pudo abrir el archivo de favoritos en la ruta especificada (" << rutaFavoritos << ").\n";
        return 1;
    }

    // Leer todos los comandos de favoritos en un vector
    std::vector<std::string> comandos;
    std::string linea;
    while (std::getline(archivoFavoritos, linea))
    {
        comandos.push_back(linea);
    }
    archivoFavoritos.close();

    bool encontrado = false;
    // Buscar y mostrar los comandos que contienen el substring buscarCmd

    int indice = 1;

    for (const auto &comando : comandos)
    {
        if (comando.find(buscarCmd) != std::string::npos)
        {
            if (!encontrado)
            {
                std::cout << "Comandos que contienen \"" << buscarCmd << "\":\n";
            }
            std::cout << indice << ". " << comando << "\n";
            encontrado = true;
        }
        indice++;
    }

    if (!encontrado)
    {
        std::cout << "No se encontraron comandos que contengan \"" << buscarCmd << "\".\n";
    }

    return 0;
}

int manejarBorrar(const std::vector<std::string> &argv)
{
    // Abre el archivo de favoritos especificado por la constante RUTA_FAVS en modo de escritura.
    std::ifstream archivofavs(RUTA_FAVS);
    if (!archivofavs)
    {
        std::cerr << "Error: no se pudo abrir el archivo de favoritos o no existe.\n";
        return 1;
    }

    std::string rutaArchivo;
    std::getline(archivofavs, rutaArchivo);
    archivofavs.close();

    if (rutaArchivo.empty())
    {
        std::cerr << "Error: no se ha creado un archivo de favoritos\n";
        return 1;
    }

    // El modo de truncamiento (std::ofstream::trunc) vacía el archivo al abrirlo.
    std::ofstream archivoParaVaciar(rutaArchivo, std::ofstream::trunc);
    if (!archivoParaVaciar)
    {
        std::cerr << "Error: no se pudo abrir el archivo especificado para vaciar.\n";
        return 1;
    }
    archivoParaVaciar.close();

    return 0;
}
/*
//ni cagando funciona pero es la idae xd
std::string manejarEjecutar(const std::vector<std::string> &argv) { //retornar la linea como string para que la ejecute la shell

    int num;
    try {
        num = std::stoi(argv[1]);  // "favs num ejecutar"  transformar "num" a int
    } catch (const std::invalid_argument&) {
        std::cerr << "Error: El primer argumento no es un número válido.\n";
        return "1";
    } catch (const std::out_of_range&) {
        std::cerr << "Error: El número proporcionado está fuera de rango.\n";
        return "1";
    }

    std::ifstream archivofavs(RUTA_FAVS);
    if (!archivofavs)
    {
        std::cerr << "Error: no se pudo abrir el archivo de favoritos o no existe.\n";
        return "1";
    }
    std::string linea;
    int lineaActual = 1;  // Contador para las líneas del archivo

    // Lee el archivo línea por línea
    while (getline(archivofavs, linea)) {
        if (lineaActual == num) {
            archivofavs.close();
            return linea;  // Retorna la línea correspondiente
        }
        lineaActual++;
    }

    archivofavs.close();  // Cierra el archivo si no se encuentra la línea
    return "Error: Número de línea fuera de rango.";  // Retorna un mensaje de error si la línea no existe

    return 0;
}*/
int manejarCargar(const std::vector<std::string> &argv) { return 0; }
int manejarGuardar(const std::vector<std::string> &argv) { return 0; }
