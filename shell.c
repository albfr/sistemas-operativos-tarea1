#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAXCOM 1000 // Máximo número de caracteres para la entrada del usuario
#define MAXLIST 100 // Máximo número de comandos
#define NUM_COMANDOS_INTERNOS 2 // Número de comandos internos soportados

const char *COMANDOS_INTERNOS[NUM_COMANDOS_INTERNOS] = {"exit", "cd"}; // Lista de comandos internos

// Función para mostrar el prompt de la shell, que incluye el nombre de la shell y el directorio actual
void propmt(){
    printf("\033[32mmishell\033[0m"); // Color del prompt en verde
    char directorioActual[1024];
    if (getcwd(directorioActual, sizeof(directorioActual)) != NULL) {
        printf(":\033[94m%s\033[0m$ ", directorioActual); // Color del directorio en azul
    } else {
        perror("Error obteniendo el directorio actual");
    }
}

// Manejador de señal para Ctrl+C
void manejarCtrlC(int sig){
    printf("\n");
    propmt();
    fflush(stdout);
}

// Función que lee la entrada del usuario
int leerEntradaUsuario(char *entrada) {
    if (fgets(entrada, MAXCOM, stdin) != NULL && strlen(entrada) > 1) {
        entrada[strlen(entrada) - 1] = '\0'; // Elimina el carácter de nueva línea
        return 0;
    }
    return 1;
}

// Función que crea un proceso hijo para ejecutar un comando utilizando execvp
void ejecutarProceso(char **argumentos) {
    pid_t pid = fork(); // Crea un nuevo proceso hijo

    if (pid < 0) { 
        perror("Error al crear el proceso hijo");
    } else if (pid == 0) { // Proceso hijo
        // Reemplaza el proceso hijo con el nuevo programa
        if (execvp(argumentos[0], argumentos) < 0) {
            printf("Orden \"%s\" no encontrada.\n", *argumentos);
            exit(EXIT_FAILURE);
        }
    } else { // Proceso padre
        wait(NULL); // Espera a que el proceso hijo termine
    }
}

// Función para manejar múltiples comandos separados por pipes
void manejarPipesMultiples(char ***comandos, int numComandos) {
    int pipefd[2], aux_fd[2]; // Descriptores de archivo para los pipes
    pid_t pid;
    int i;

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
            ejecutarProceso(comandos[i]); // Ejecuta el comando
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
int manejarComandosInternos(char **argumentos) {
    for (int i = 0; i < NUM_COMANDOS_INTERNOS; i++) {
        if (strcmp(argumentos[0], COMANDOS_INTERNOS[i]) == 0) {
            switch (i) {
                case 0: // "exit"
                    printf("Saliendo de la terminal...\n");
                    exit(0);
                case 1: // "cd"
                    if (chdir(argumentos[1]) != 0) {
                        perror("Error al cambiar de directorio");
                    }
                    return 1;
            }
        }
    }
    return 0;
}

// Función que separa la cadena de entrada en comandos individuales utilizando pipes
void separarComandosPorPipe(char *cadena, char ***comandos, int *numComandos) {
    *numComandos = 0;
    char *comando;

    // Divide la cadena en comandos utilizando el delimitador "|"
    while ((comando = strsep(&cadena, "|")) != NULL) {
        comandos[*numComandos] = (char **)malloc(MAXLIST * sizeof(char *));
        int i = 0;
        char *arg;
        // Divide cada comando en argumentos individuales
        while ((arg = strsep(&comando, " ")) != NULL) {
            if (*arg == '\0') continue;
            comandos[*numComandos][i++] = arg;
        }
        comandos[*numComandos][i] = NULL;
        (*numComandos)++;
    }
}

// Función principal que procesa la cadena de entrada y decide cómo manejarla (comando interno o pipe)
int procesarCadenaEntrada(char *cadena, char ***comandos, int *numComandos) {
    separarComandosPorPipe(cadena, comandos, numComandos);

    // Verifica si es un comando interno y lo maneja si es necesario
    if (*numComandos == 1 && manejarComandosInternos(comandos[0]))
        return 0;
    return *numComandos;
}

int main() {
    char cadenaEntrada[MAXCOM]; // Buffer para la entrada del usuario
    char **comandos[MAXLIST]; // Arreglo para almacenar los comandos separados por pipes
    int numComandos; 

    // Configurar el manejador de señales para Ctrl + C
    signal(SIGINT, manejarCtrlC);

    // Inicialización y bienvenida
    printf("\033[H\033[J"); 
    printf("Abriendo terminal...\n");
    sleep(1);
    printf("\033[H\033[J");

    // Bucle principal de la shell
    while (1) {
        propmt(); // Mostrar el prompt

        if (leerEntradaUsuario(cadenaEntrada)) // Leer la entrada del usuario
            continue;

        numComandos = procesarCadenaEntrada(cadenaEntrada, comandos, &numComandos); // Procesar la entrada

        if (numComandos > 0)
            manejarPipesMultiples(comandos, numComandos); // Manejar los comandos si hay pipes

        for (int i = 0; i < numComandos; i++) { // Liberar la memoria utilizada
            free(comandos[i]);
        }
    }
    return 0;
}
