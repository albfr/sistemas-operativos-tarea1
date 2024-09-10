#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <algorithm>
#include <errno.h>
#include "favs/favs.h"

#define MAXCOM 1000 // Máximo número de caracteres para la entrada del usuario
#define MAXLIST 100 // Máximo número de comandos
#define NUM_COMANDOS_INTERNOS 3 // Número de comandos internos soportados

// const char *COMANDOS_INTERNOS[NUM_COMANDOS_INTERNOS] = {"exit", "cd"}; // Lista de comandos internos
const std::vector<std::string> COMANDOS_INTERNOS = {"exit", "cd", "favs"};
std::vector<std::string> historial;

// Función para mostrar el prompt de la shell, que incluye el nombre de la shell y el directorio actual
void prompt(){
    printf("\033[32mmishell\033[0m:$ "); // Color del prompt en verde
}

// Manejador de señal para Ctrl+C
void manejarCtrlC(int sig){
    printf("\n");
    prompt();
    fflush(stdout);
}

// Función que lee la entrada del usuario
// int leerEntradaUsuario(char *entrada) {
int leerEntradaUsuario(std::string &line) {
    getline(std::cin, line);
    return 0;
}

char** vvstring_to_vvcstring(std::vector<std::string> &argumentos) {
    char **argumentos_c = (char**)malloc(sizeof(char*) * argumentos.size());
    for (int i = 0; i < argumentos.size(); i++) {
        int m = argumentos[i].length();
        argumentos_c[i] = (char*)malloc(sizeof(char) * (argumentos[i].length()+1));
        for (int j=0;j<m;j++) {
            argumentos_c[i][j] = argumentos[i][j];
        }
        argumentos_c[i][m] = '\0';
    }
    return argumentos_c;
}

void free_vvcstring(char **vvcstring, int n) {
    for (int i = 0; i < n; i++) {
        free(vvcstring[i]);
    }
    free(vvcstring);
}

// Función que crea un proceso hijo para ejecutar un comando utilizando execvp
// void ejecutarProceso(char **argumentos) {
void ejecutarProceso(std::vector<std::string> &argumentos) {
    pid_t pid = fork(); // Crea un nuevo proceso hijo
    int maxsize = 0;
    for (int i = 0; i < argumentos.size(); i++)
        maxsize = std::max(maxsize, (int)argumentos[i].length());

    // char **argumentos_c = (char**)malloc(sizeof(char**) * argumentos.size());

    if (pid < 0) { 
        perror("Error al crear el proceso hijo");
    }
    else if (pid == 0) { // Proceso hijo

        if (argumentos[0].compare(COMANDOS_INTERNOS[2]) != 0)
        {
            // Reemplaza el proceso hijo con el nuevo programa
            char **argumentos_c = vvstring_to_vvcstring(argumentos);
            int error = execvp(argumentos_c[0], argumentos_c);
            free_vvcstring(argumentos_c, argumentos.size());

            if (error < 0) {
                fprintf(stderr, "errno = %d\n", errno);
                printf("Orden \"%s\" no encontrada.\n", argumentos_c[0]);
                exit(EXIT_FAILURE);
            }
        }
    }
    else { // Proceso padre
        wait(NULL); // Espera a que el proceso hijo termine
    }
}

// Función para manejar múltiples comandos separados por pipes
// void manejarPipesMultiples(char ***comandos, int numComandos) {
void manejarPipesMultiples(std::vector<std::vector<std::string>> &comandos) {
    int pipefd[2], aux_fd[2]; // Descriptores de archivo para los pipes
    pid_t pid;
    int i;

    int numComandos = comandos.back().size();
    for (i = 0; i < numComandos; i++) {
        // Crear pipe si no es el último comando
        if (i < numComandos - 1) {
            if (pipe(pipefd) == -1) {
                perror("Error al crear pipe");
                exit(EXIT_FAILURE);
            }
        }

        pid = fork(); // Crea un nuevo proceso hijo para ejecutar el comando
        if (pid < 0) { // Error al crear el proceso hijo
            perror("Error al crear el proceso hijo");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Proceso hijo
            if (i > 0) { // No es el primer comando, lee de aux_fd (pipe anterior)
                dup2(aux_fd[0], STDIN_FILENO); // Redirige la entrada estándar al pipe anterior
                close(aux_fd[0]);
                close(aux_fd[1]);
            }
            if (i < numComandos - 1) { // No es el último comando, escribe en pipefd
                dup2(pipefd[1], STDOUT_FILENO); // Redirige la salida estándar al nuevo pipe
                close(pipefd[0]);
                close(pipefd[1]);
            }
            ejecutarProceso(comandos.back()); // Ejecuta el comando
            exit(EXIT_SUCCESS);
        } else { // Proceso padre
            if (i > 0) { // Cierra el pipe anterior después de que el hijo lo haya usado
                close(aux_fd[0]);
                close(aux_fd[1]);
            }
            if (i < numComandos - 1) { // Guarda pipefd para el siguiente comando
                aux_fd[0] = pipefd[0];
                aux_fd[1] = pipefd[1];
            }
            wait(NULL); // Espera a que el proceso hijo termine
        }
    }
}


// Función que maneja la ejecución de comandos internos como "exit" y "cd"
// int manejarComandosInternos(char **argumentos) {
int manejarComandosInternos(std::vector<std::string> &argumentos) {
    int ejecutado = 0;
    for (int i = 0; i < NUM_COMANDOS_INTERNOS; i++) {
        if (argumentos[0].compare(COMANDOS_INTERNOS[i]) == 0) {
            switch (i) {
                case 0: // "exit"
                    printf("Saliendo de la terminal...\n");
                    exit(0);
                    break;
                case 1: // "cd"
                    if (chdir(argumentos[1].c_str()) != 0)
                        perror("Error al cambiar de directorio");
                    return 1;
                case 2: // "favs"
                    favs_process(argumentos, historial);
                    break;
            }
        }
    }
    return 0;
}

// Función que separa la cadena de entrada en comandos individuales utilizando pipes
// void separarComandosPorPipe(char *cadena, char ***comandos, int *numComandos) {
void separarComandosPorPipe(std::string &cadena, std::vector<std::vector<std::string>> &comandos) {
    // char *comando;
    std::string comando;

    // Divide la cadena en comandos utilizando el delimitador "|"
    // while ((comando = strsep(&cadena, "|")) != NULL) {
    cadena += '|';
    int prev = 0;
    int pos = cadena.find('|');
    while (pos != std::string::npos) {
        comando = cadena.substr(prev, pos-prev) + ' ';
        comandos.push_back(std::vector<std::string>(0));
        // comandos[*numComandos] = (char **)malloc(MAXLIST * sizeof(char *));

        int i = 0;
        char *arg;
        int first = 0;
        int second = comando.find(' ');
        while (second != std::string::npos) {
            comandos.back().push_back(comando.substr(first, second-first));
            first = second + 1;
            second = comando.find(" ", second+1, 1);
        }
        prev = pos + 1;
        pos = cadena.find('|', pos+1);
    }
    cadena.pop_back();
}

// Función principal que procesa la cadena de entrada y decide cómo manejarla (comando interno o pipe)
// int procesarCadenaEntrada(char *cadena, char ***comandos, int *numComandos) {
int procesarCadenaEntrada(std::string &cadena, std::vector<std::vector<std::string>> &comandos) {
    separarComandosPorPipe(cadena, comandos);

    // Verifica si es un comando interno y lo maneja si es necesario
    if (comandos.back().size() > 0 && manejarComandosInternos(comandos.back()))
        return 1;
    return 0;
}

int main() {
    // char cadenaEntrada[MAXCOM]; // Buffer para la entrada del usuario
    std::string cadenaEntrada;
    // char **comandos[MAXLIST]; // Arreglo para almacenar los comandos separados por pipes
    std::vector<std::vector<std::string>> comandos;
    int numComandos; 

    // Configurar el manejador de señales para Ctrl + C
    signal(SIGINT, manejarCtrlC);

    // Inicialización y bienvenida
    printf("\033[H\033[J"); 
    printf("Abriendo terminal...\n");
    sleep(0.3);
    printf("\033[H\033[J");

    int ctp[2], ptc[2];
    if (pipe(ctp) == -1) {
        perror("Ocurrió un error al abrir pipe\n");
        return 1;
    }
    if (pipe(ptc) == -1) {
        perror("Ocurrió un error al abrir pipe\n");
        return 1;
    }
    int id = fork();

    std::vector<std::string> history; // favs dinamico
    while (true) {
        prompt(); // Mostrar el prompt

        if (leerEntradaUsuario(cadenaEntrada)) // Leer la entrada del usuario
            continue;
        
        bool exists = false;
        for (int i = 0; i < history.size(); i++) {
            if (history[i].compare(cadenaEntrada) == 0) {
                exists = true;
                break;
            }
        }
        if (!exists)
            history.push_back(cadenaEntrada);
        
        int ejecutado = procesarCadenaEntrada(cadenaEntrada, comandos); // Procesar la entrada

        // for (int i = 0; i < comandos.size(); i++) {
        //     for (int j = 0; j < comandos[i].size(); j++) {
        //         std::cout << "'" << comandos[i][j] << "' ";
        //     }
        //     std::cout << "\n";
        // }

        // favs no aguanta comunicación con pipe
        /*
        if (!ejecutado and comandos.size() == 1 and comandos[0][0].compare("favs") == 0) {
            comandos[0][0] = "./favs/favs";
            char **argumentos_c = vvstring_to_vvcstring(comandos[0]);
            int error = execvp(argumentos_c[0], argumentos_c);
            free_vvcstring(argumentos_c, comandos[0].size());
            ejecutado = 1;
        }
        */

        if (!ejecutado) {
            manejarPipesMultiples(comandos); // Manejar los comandos si hay pipes
        }

        comandos.clear();
    }
    return 0;
}
