#include <stdio.h>      // Biblioteca estándar para entrada/salida.
#include <stdlib.h>     // Biblioteca estándar para manejo de memoria dinámica y utilidades.
#include <fcntl.h>      // Para flags de control de archivos.
#include <sys/stat.h>   // Para manejo de permisos de archivos.
#include <unistd.h>     // Para llamadas al sistema como open, read, write y close.
#include <errno.h>      // Para manejo de errores.
#include <string.h>     // Para manipulación de cadenas y funciones como strcpy.

// Definimos la estructura del alumno según el enunciado
typedef struct alumno {
    char nombre[50];  // Nombre del alumno (máx. 50 caracteres).
    int nota;         // Nota del alumno.
    int convocatoria; // Convocatoria en la que se presentó.
} Alumno;

#define MAX_ALUMNOS 100 // Número máximo de alumnos permitido.

// Función de comparación para ordenar los alumnos por nota.
int comparar_por_nota(const void *a, const void *b) {
    return ((Alumno *)a)->nota - ((Alumno *)b)->nota;
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

    Alumno alumnos[MAX_ALUMNOS];
    int num_alumnos = 0;

    // Intentamos abrir el primer archivo en modo lectura binaria
    int fd1 = open(archivo1, O_RDONLY);
    if (fd1 < 0) {
        perror("Error al abrir el primer archivo");
        return -1;
    }

    // Intentamos abrir el segundo archivo en modo lectura binaria
    int fd2 = open(archivo2, O_RDONLY);
    if (fd2 < 0) {
        perror("Error al abrir el segundo archivo");
        close(fd1);
        return -1;
    }

    // Leer alumnos del primer archivo
    Alumno temp;
    while (read(fd1, &temp, sizeof(Alumno)) == sizeof(Alumno)) {
        if (num_alumnos >= MAX_ALUMNOS) {
            perror("Error: Se ha excedido el número máximo de alumnos permitido.");
            close(fd1);
            close(fd2);
            return -1;
        }
        alumnos[num_alumnos++] = temp;
    }
    close(fd1);

    // Leer alumnos del segundo archivo
    while (read(fd2, &temp, sizeof(Alumno)) == sizeof(Alumno)) {
        if (num_alumnos >= MAX_ALUMNOS) {
            perror("Error: Se ha excedido el número máximo de alumnos permitido.");
            close(fd2);
            return -1;
        }
        alumnos[num_alumnos++] = temp;
    }
    close(fd2);

    // Ordenamos los alumnos por nota de menor a mayor
    qsort(alumnos, num_alumnos, sizeof(Alumno), comparar_por_nota);

    // Creamos el archivo de salida en modo binario
    int fd_out = creat(archivo_salida, 0666);
    if (fd_out < 0) {
        perror("Error al crear el archivo de salida");
        return -1;
    }

    // Escribimos los alumnos ordenados en el archivo de salida
    for (int i = 0; i < num_alumnos; i++) {
        write(fd_out, &alumnos[i], sizeof(Alumno));
    }
    close(fd_out);

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

    // Creamos el archivo de estadísticas en modo texto
    FILE *estadisticas = fopen(archivo_estadisticas, "w");
    if (!estadisticas) {
        perror("Error al crear el archivo de estadísticas");
        return -1;
    }

    // Escribimos en el archivo de estadísticas
    fprintf(estadisticas, "M;%d;%.2f%%\n", categoria_M, (categoria_M * 100.0) / num_alumnos);
    fprintf(estadisticas, "S;%d;%.2f%%\n", categoria_S, (categoria_S * 100.0) / num_alumnos);
    fprintf(estadisticas, "N;%d;%.2f%%\n", categoria_N, (categoria_N * 100.0) / num_alumnos);
    fprintf(estadisticas, "A;%d;%.2f%%\n", categoria_A, (categoria_A * 100.0) / num_alumnos);
    fprintf(estadisticas, "F;%d;%.2f%%\n", categoria_F, (categoria_F * 100.0) / num_alumnos);

    fclose(estadisticas);

    return 0; // Ejecución exitosa
}