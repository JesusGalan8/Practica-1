#include <stdio.h>    // Biblioteca estándar para entrada y salida (printf, perror, etc.).
#include <stdlib.h>   // Biblioteca estándar para funciones como strtol (conversión de string a número).
#include <fcntl.h>    // Contiene las definiciones para manipulación de archivos (open, creat, etc.).
#include <unistd.h>   // Proporciona acceso a la API del sistema operativo (close, umask, etc.).
#include <sys/stat.h>
#include <errno.h>    // Contiene las definiciones de códigos de error del sistema.
#include <string.h>   // Para manipulación de cadenas, aunque en este código no se usa directamente.


int main(int argc, char *argv[]) {
    // Verifica que el número de argumentos pasados al programa sea exactamente 3 (nombre del programa + 2 argumentos).
    if (argc != 3) {
        perror("error in the number of arguments"); // Muestra un mensaje de error si los argumentos son incorrectos.
        return -1; // Finaliza el programa con un código de error.
    }

    //Aqui va
    if (strlen(argv[2])!=4 || argv[2][0] != '0'){
        perror("Error: los permisos deben ser una cadena de 4 caracteres empezada por 0"); // Muestra un mensaje de error si los argumentos son incorrectos.
        return -2;
    }

    char *nombre = argv[1]; // Guarda el primer argumento como el nombre del archivo a crear.
    int fd; // Variable para almacenar el descriptor de archivo.
    mode_t mode; // Variable para almacenar los permisos del archivo.
    char *error; // Puntero que usaremos para detectar errores

    /*Aqui le decimos que vamos a coger la cadena agrv y concretamente la posición 2 (donde se almacena el permiso). En la dirección de 
    de error guardamos el primer caracter que no cumple con el octal. Si es correcto, el primer caracter que no cumplicará será '/0', 
    ya que los datos se pasan en caracteres.*/
    mode = strtol(argv[2], &error, 8);

    /* Por lo dicho anteriormente, si el caracter que no cumple con octal es '\0', es correcto, ya que significa que hemos llegado al fin.
    Sin embargo, si encontramos cualquier otro caracter diferente a este significa que la entrada no cumple con el octal, por lo que debemos
    lanzar un error.*/
    if (*error != '\0') {
        perror("Error: No se pudo convertir a octal");
        return -3;
    }

    // Guarda la máscara de permisos actual y la establece temporalmente en 0 para que los permisos especificados se apliquen correctamente.
    mode_t mascara_vieja = umask(0);

    // Crea un archivo con el nombre y permisos especificados por el usuario.
    // 'creat' equivale a open con los flags O_CREAT | O_WRONLY | O_TRUNC.
    if ((fd = creat(nombre, mode)) < 0) {
        perror("error creating a file"); // Muestra un mensaje de error si la creación del archivo falla.
        return (-4); // Finaliza el programa con un código de error.
    }

    close(fd); // Cierra el archivo recién creado para liberar recursos.
    umask(mascara_vieja); // Restaura la máscara de permisos original del sistema.

    return 0; // Finaliza el programa con éxito.
}


