#include <stdio.h>      // Biblioteca estándar para entrada/salida.
#include <stdlib.h>     // Biblioteca estándar para manejo de memoria dinámica y utilidades.
#include <fcntl.h>      // Para flags de control de archivos.
#include <sys/stat.h>   // Para manejo de permisos de archivos.
#include <unistd.h>     // Para llamadas al sistema como open, read, write y close.
#include <errno.h>      // Para manejo de errores.
#include <string.h>     // Para manipulación de cadenas y funciones como strcpy.

#define MAX_ALUMNOS 100 // Número máximo de alumnos permitido.

// Definimos la estructura del alumno según el enunciado
struct alumno {
    char nombre[50];  // Nombre del alumno (máx. 50 caracteres).
    int nota;         // Nota del alumno.
    int convocatoria; // Convocatoria en la que se presentó.
};
typedef struct alumno tAlumno;

// Función para intercambiar dos elementos en el array
void swap(tAlumno *a, tAlumno *b) {
    tAlumno temp = *a;
    *a = *b;
    *b = temp;
}

// QuickSort adaptado
void quick_sort(tAlumno arr[], int start, int end) {
    if (start >= end) return;

    tAlumno p = arr[end]; // Pivote
    int i = start, j = end;

    while (i <= j) {
        while (arr[i].nota < p.nota) i++; // Ahora ordena de menor a mayor
        while (arr[j].nota > p.nota) j--;

        if (i <= j) {
            swap(&arr[i], &arr[j]);
            i++;
            j--;
        }
    }

    if (j > start) quick_sort(arr, start, j);
    if (i < end) quick_sort(arr, i, end);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        perror("Número de argumentos incorrecto. Uso: ./combine <archivo1> <archivo2> <archivo_salida>");
        return -1;
    }

    char *archivo1 = argv[1];
    char *archivo2 = argv[2];
    char *archivo_salida = argv[3];
    char *archivo_estadisticas = "estadisticas.csv"; // Nombre fijo para estadísticas
    int fd_archivo1, fd_archivo2, fd_salida, fichero_estadisticas; 

    tAlumno alumnos[MAX_ALUMNOS];
    int num_alumnos = 0;

    if ((fd_archivo1 = open(archivo1,O_RDONLY))<0) {
        perror("Error: no se ha podido abrir el primer fichero");
        return -2;
    }

    if ((fd_archivo2 = open(archivo2,O_RDONLY))<0) {
        perror("Error: no se ha podido abrir el segundo fichero");
        close(fd_archivo1);
        return -3;
    }

    // Leer alumnos del primer archivo
    tAlumno temp;
    while (read(fd_archivo1, &temp, sizeof(tAlumno)) == sizeof(tAlumno)) {
        if (num_alumnos >= MAX_ALUMNOS) {
            perror("Error: Se ha excedido el número máximo de alumnos permitido.");
            close(fd_archivo1);
            close(fd_archivo2);
            return -4;
        }
        alumnos[num_alumnos++] = temp;
    }
    close(fd_archivo1);

    // Leer alumnos del segundo archivo
    while (read(fd_archivo2, &temp, sizeof(tAlumno)) == sizeof(tAlumno)) {
        if (num_alumnos >= MAX_ALUMNOS) {
            perror("Error: Se ha excedido el número máximo de alumnos permitido.");
            close(fd_archivo2);
            return -4;
        }
        alumnos[num_alumnos++] = temp;
    }
    close(fd_archivo2);

    // Ordenamos los alumnos por nota de menor a mayor
    quick_sort(alumnos, 0,num_alumnos-1);

    if ((fd_salida = open(archivo_salida, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
        perror("Error: no se ha podido abrir el fichero de salida");
        return -5;
    }
 
    for (int i = 0; i < num_alumnos; i++) {
        if (write(fd_salida, &alumnos[i], sizeof(tAlumno)) != sizeof(tAlumno)) {
            perror("Error al escribir en el archivo de salida");
            close(fd_salida);
            return -6;
        }
    }
    close(fd_salida);

    // Contadores de estadísticas
    int categoria_M = 0, categoria_S = 0, categoria_N = 0, categoria_A = 0, categoria_F = 0;

    // Clasificamos los alumnos por su nota
    for (int i = 0; i < num_alumnos; i++) {
        if (alumnos[i].nota == 10)
            categoria_M++;
        else if (alumnos[i].nota == 9)
            categoria_S++;
        else if (alumnos[i].nota >= 7)
            categoria_N++;
        else if (alumnos[i].nota >= 5)
            categoria_A++;
        else
            categoria_F++;
    }

    if ((fichero_estadisticas = creat(archivo_estadisticas, 0666)) < 0) {
        perror("Error: no se pudo generar el fichero con las estadísticas");
        return -7;
    }
    
       
    char buffer[100];
    int len;
    /*La función snintf, lo que hace es guardarnos en memoria una cadena del tamaño buffer. En esta función, lo primero que debemos pasar es la cadena
    donde se almacenará el resultado, en este caso, buffer. Seguidamente le pasamos el tamaño del buffer, para que sepa cual es el número máximo de 
    caracteres que puede almacenar. Tras esto, le pasamos el formato de la cadena, que este caos será M seguido de un entero y un número en punto flotante,
    que estos será el número de dicha estadisca y el porcentaje de la clase que obtuvo dicha calificación */
    len = snprintf(buffer, sizeof(buffer), "M;%d;%.2f%%\n", categoria_M, (categoria_M * 100.0) / num_alumnos);
    if (write(fichero_estadisticas, buffer, len) < 0) {
        perror("Error al escribir estadísticas");
        return -8;
    }
    
    len = snprintf(buffer, sizeof(buffer), "S;%d;%.2f%%\n", categoria_S, (categoria_S * 100.0) / num_alumnos);
    if (write(fichero_estadisticas, buffer, len) < 0) {
        perror("Error al escribir estadísticas");
        return -9;
    }
    
    len = snprintf(buffer, sizeof(buffer), "N;%d;%.2f%%\n", categoria_N, (categoria_N * 100.0) / num_alumnos);
    if (write(fichero_estadisticas, buffer, len) < 0) {
        perror("Error al escribir estadísticas");
        return -10;
    }
    
    len = snprintf(buffer, sizeof(buffer), "A;%d;%.2f%%\n", categoria_A, (categoria_A * 100.0) / num_alumnos);
    if (write(fichero_estadisticas, buffer, len) < 0) {
        perror("Error al escribir estadísticas");
        return -11;
    }
    
    len = snprintf(buffer, sizeof(buffer), "F;%d;%.2f%%\n", categoria_F, (categoria_F * 100.0) / num_alumnos);
    if (write(fichero_estadisticas, buffer, len) < 0) {
        perror("Error al escribir estadísticas");
        return -12;
    }
       
    close(fichero_estadisticas);
    return 0; 
}