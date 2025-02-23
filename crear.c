#include <stdio.h>    // Biblioteca estándar para entrada y salida (printf, perror, etc.).
#include <stdlib.h>   // Biblioteca estándar para funciones como strtol (conversión de string a número).
#include <fcntl.h>    // Contiene las definiciones para manipulación de archivos (open, creat, etc.).
#include <sys/stat.h> // Contiene definiciones de estructuras y constantes para permisos de archivos.
#include <unistd.h>   // Proporciona acceso a la API del sistema operativo (close, umask, etc.).
#include <errno.h>    // Contiene las definiciones de códigos de error del sistema.
#include <string.h>   // Para manipulación de cadenas, aunque en este código no se usa directamente.

int main(int argc, char *argv[]) {
    // Verifica que el número de argumentos pasados al programa sea exactamente 3 (nombre del programa + 2 argumentos).
    if (argc != 3) {
        perror("error in the number of arguments"); // Muestra un mensaje de error si los argumentos son incorrectos.
        return -1; // Finaliza el programa con un código de error.
    }

    char *nombre = argv[1]; // Guarda el primer argumento como el nombre del archivo a crear.
    int fd; // Variable para almacenar el descriptor de archivo.
    mode_t mode; // Variable para almacenar los permisos del archivo.
    char *endptr; // Puntero para manejar errores en la conversión de string a número.

    // Convierte el segundo argumento (cadena de caracteres) a un número en base 8 (octal) y lo almacena en mode.
    mode = strtol(argv[2], &endptr, 8);

    // Guarda la máscara de permisos actual y la establece temporalmente en 0 para que los permisos especificados se apliquen correctamente.
    mode_t old_mask = umask(0);

    // Crea un archivo con el nombre y permisos especificados por el usuario.
    // 'creat' equivale a open con los flags O_CREAT | O_WRONLY | O_TRUNC.
    if ((fd = creat(nombre, mode)) < 0) {
        perror("error creating a file"); // Muestra un mensaje de error si la creación del archivo falla.
        return (-1); // Finaliza el programa con un código de error.
    }

    close(fd); // Cierra el archivo recién creado para liberar recursos.
    umask(old_mask); // Restaura la máscara de permisos original del sistema.

    return 0; // Finaliza el programa con éxito.
}



