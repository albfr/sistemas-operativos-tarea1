# Tarea 1
A continuación se hace una breve descripción sobre cómo compilar y ejecutar los comandos solicitados.
## Shell
### compilación:
Se necesita un sistema Unix, su compilación y ejecución es como la de cualquier programa:

```bash
g++ shell.cpp -o shell 
./shell
```
y, se debería mostrar el prompt en pantalla.

### Uso de la shell:
La shell 'mishell' funciona de manera similar a la terminal por defecto del sistema en:
- Comandos internos (`cd`, `ls`, `exit`, etc.).
- Soporte de indefinida cantidad de argumentos para los comandos.
- Soporte de diversos pipes´.

## Favs
El comando personalizado 'favs' tiene una opción de 'favs ayuda' para mostrar el uso correcto de este
- Despligue de especificaciones (crear, mostrar, eliminar, etc).

### Interpretación sobre 'memoria' para favs:
- Favs Dinámico: La shell tiene una 'memoria' propia o historial, a la que nos referiremos como `fd`.
- Favs Estático: tiene un archivo .txt que es estático (cambia solo cuando el usuario quiere que cambie), digamos `fe`.

Cada vez que el usuario ejecute un comando en la shell, este se guarda en la `fd`, como el historial de una terminal de sistema cualquiera.

Cuando el usuario quiera, puede guardar ese historial existente en la sesión actual y llevarla a su documento .txt, pasa la `fd` a la `fe`.

También, puede traer lo de `fe` a `fd` con `favs cargar`.

El usuario, cuando hace uso de `favs mostrar`, muestra tanto el historial de la sesión actual `fd` como `fe`.


## Recordatorio (set)
El comando personalizado 'set' tiene una opción de 'set ayuda' para mostrar el uso correcto de este
- Despliegue de especificaciones (ayuda, recordatorio)

Donde, recordatorio se llama de la forma:
```bash
set recordatorio [tiempo][nombreRecordatorio]
```
Donde tiempo se interpreta en segundos.