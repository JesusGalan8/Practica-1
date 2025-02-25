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

    char archivo1 = argv[1];
    char archivo2 = argv[2];
    char archivo_salida = argv[3];
    char *archivo_estadisticas = "estadisticas.csv"; // Nombre fijo para estadísticas

    tAlumno alumnos[MAX_ALUMNOS];
    int num_alumnos = 0;

    if ((archivo1 = open(argv[1],O_RDONLY))<0) {
        perror("Error: no se ha podido abrir el primer fichero");
        return -2;
    }

    if ((archivo2 = open(argv[2],O_RDONLY))<0) {
        perror("Error: no se ha podido abrir el segundo fichero");
        close(archivo1);
        return -3;
    }

    // Leer alumnos del primer archivo
    tAlumno temp;
    while (read(archivo1, &temp, sizeof(tAlumno)) == sizeof(tAlumno)) {
        if (num_alumnos >= MAX_ALUMNOS) {
            perror("Error: Se ha excedido el número máximo de alumnos permitido.");
            close(archivo1);
            close(archivo2);
            return -4;
        }
        alumnos[num_alumnos++] = temp;
    }
    close(archivo1);

    // Leer alumnos del segundo archivo
    while (read(archivo2, &temp, sizeof(tAlumno)) == sizeof(tAlumno)) {
        if (num_alumnos >= MAX_ALUMNOS) {
            perror("Error: Se ha excedido el número máximo de alumnos permitido.");
            close(archivo2);
            return -4;
        }
        alumnos[num_alumnos++] = temp;
    }
    close(archivo2);

    // Ordenamos los alumnos por nota de menor a mayor
    quick_sort(alumnos, 0,num_alumnos-1);

    if ((archivo_salida = open(argv[3],O_WRONLY | O_CREAT | O_TRUNC))<0) {
        perror("Error: no se ha podido abrir el fichero de retorno");
        return -5;
    }

    for (int i = 0; i < num_alumnos; i++) {
        write(num_alumnos, &alumnos[i], sizeof(tAlumno));
    }

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

    int fichero_estadisticas;
    if (fichero_estadisticas = creat(archivo_estadisticas,0666) < 1){
        perror("Error: no se pudo generar el fichero con las estadístcias");
        return -6;
    }
    // Creamos el archivo de estadísticas en modo texto
    
    return 0; // Ejecución exitosa
}