#include <stdio.h>      // Biblioteca estándar para entrada/salida.
#include <stdlib.h>     // Biblioteca estándar para manejo de memoria dinámica y utilidades.
#include <fcntl.h>      // Para flags de control de archivos.
#include <sys/stat.h>   // Para manejo de permisos de archivos.
#include <unistd.h>     // Para llamadas al sistema como open, read, write y close.
#include <errno.h>      // Para manejo de errores.
#include <string.h>     // Para manipulación de cadenas y funciones como strcpy.

#define MAX_ALUMNOS 100 // Número máximo de alumnos permitido.

struct alumno {
    char nombre[50];  // Nombre del alumno (máx. 50 caracteres).
    int nota;         // Nota del alumno.
    int convocatoria; // Convocatoria en la que se presentó.
};
typedef struct alumno tAlumno;

/*Esta función sirve para pasarle al qsort como debe comparar los elementos que hay dentro de tAlumno. Esta función
recibirá dos parametros de tipo const void, ya que la función no sabe que tipo de datos está ordenando*/
int comparacion(const void *a, const void *b) {  
    //Una vez comenzamos la función convertimos estos al tAlumno, para que sean del tipo de la estructura y poder
    // acceder así a la nota.              
    const tAlumno *alumnoA = (const tAlumno *)a;
    const tAlumno *alumnoB = (const tAlumno *)b;
    /*Finalmente comparamos la nota de cada alumno y las restamos. Si el resultado es negativo, entonces el qsort podrá
    al Alumno A antes que al B. Si el resultado es 0, entonces significa que ambas notas son iguales y da igual el orden.
    Finalmente, si el resultado es positivo, se podrá al alumnoB antes del alumno A*/
    return alumnoA->nota - alumnoB->nota;
}

int main(int argc, char *argv[]) {
    // Comprobamos que la función recibe 4 parametros
    if (argc != 4) {
        perror("Número de argumentos incorrecto");
        return -1;
    }

    // Creamos las variables archivo1, archivo 2 y el de salida para almacenar el nombre de los ficheros que vamos a recibir
    char *archivo1 = argv[1];
    char *archivo2 = argv[2];
    char *archivo_salida = argv[3];
    // Creamos el archivo de estadísticas con el formato pedido
    char *archivo_estadisticas = "estadisticas.csv"; 
    // Creamos donde vamos a almacenar los ficheros que vayamos a abrir
    int fd_archivo1, fd_archivo2, fd_salida, fichero_estadisticas; 

    // Definimos la estructra alumnos donde guardaremos todos los alumnos que vayamos leyendo en los ficheros de entrada
    tAlumno alumnos[MAX_ALUMNOS];
    // creamos una variable para contabilizar el número de alumnos y así asegurarnos que nunca haya más de 100 alumnos
    int num_alumnos = 0;

    // Abrimos el primer fichero que nos han pasado con permisos de solo lectura y lo almacenamos en fd_archivo1
    if ((fd_archivo1 = open(archivo1,O_RDONLY))<0) {
        perror("Error: no se ha podido abrir el primer fichero");
        return -2;
    }

    // Abrimos el segundo fichero que nos han pasado con permisos de solo lectura y lo almacenamos en fd_archivo2
    if ((fd_archivo2 = open(archivo2,O_RDONLY))<0) {
        perror("Error: no se ha podido abrir el segundo fichero");
        // En caso de haber un error debemos cerrar los ficheros que hemos abierto previamente
        close(fd_archivo1);
        return -3;
    }

    // Creamos una variable temporal del tipo de la estructura
    tAlumno x;
    /*Vamos a leer lo que se encuentre en el primer fichero que nos han mandado y vamos a guardar tantos elementos como 
    el tamñao de la estructura en la dirección que apunta x, es decir la estructura temporasl que hemos creado. Leeremso siempre y cuando
    el número de elementos leidos sea igual al tamaño de la estructura, es decir siempre y cuando queden elementos en la estructura */
    while (read(fd_archivo1, &x, sizeof(tAlumno)) == sizeof(tAlumno)) {
        // En caso de algún momento superar el máximo de 100 alumnos entonces cerraremos los ficheros y lanzaremos un error
        if (num_alumnos >= MAX_ALUMNOS) {
            perror("Error: Se ha excedido el número máximo de alumnos permitido.");
            close(fd_archivo1);
            close(fd_archivo2);
            return -4;
        }
        // Por cada paso vamos guardando los elemntos leidos en la estructura creada al principio. Asimismo, aumentamos en 1 el número de alumnos
        // contabilizando así el número de alumnos que hay en una clase
        alumnos[num_alumnos++] = x;
    }
    // Como ya hemos leido todos los datos que hay en el primer fichero de entrada podemos cerarlo ya
    close(fd_archivo1);

    // Para leer los datos del segundo fichero solo tenemos que repetir lo que hemos hecho para leer los del primer fichero
    while (read(fd_archivo2, &x, sizeof(tAlumno)) == sizeof(tAlumno)) {
        if (num_alumnos >= MAX_ALUMNOS) {
            perror("Error: Se ha excedido el número máximo de alumnos permitido.");
            close(fd_archivo2);
            return -4;
        }
        alumnos[num_alumnos++] = x;
    }
    // Como ya hemos leido todos los datos que hay en el segundo fichero de entrada podemos cerarlo ya
    close(fd_archivo2);

    // Ordenamos los alumnos por nota de menor a mayor mediante un quick sort
    /*Esta función lo que hace ordenar mediante un quicksort una lista de alumnos, con numalumnos elementos
    y sizeof(tAlumno) como el tamaño de cada elemento. Finalmente le tenemos que pasar una función comparación para
    que sepa como tiene que comparar para saber cual es mayor o menor */
    qsort(alumnos, num_alumnos, sizeof(tAlumno), comparacion);

    // Abrimos el fichero de salida en modo escritura, en caso de que no exista lo crearemos o si existe pero está escrito, entonces
    // lo truncaremos. Por esta misma razón también escribimos los permismos de dicho archivo
    if ((fd_salida = open(archivo_salida, O_WRONLY | O_CREAT | O_TRUNC,0666)) < 0) {
        perror("Error: no se ha podido abrir el fichero de salida");
        return -5;
    }
    
    // Vamos a iterrar por todos los alumnos, para así meter a todos los alumnos en el archivo de salida
    for (int i = 0; i < num_alumnos; i++) {
        //Vamos a escribir valores provenientes de la estructa alumnos, y cada elemento que escribamos debe tener el tamaño de la estructrua. En caso 
        // de que escribamos menos elementos que el tamaño de la estructura significaría que ha habido un error en la escritura, ya que tenemos contabilizado
        // el número de alumnos, por lo que significaria un error, no que haya terminado de escribir
        if (write(fd_salida, &alumnos[i], sizeof(tAlumno)) != sizeof(tAlumno)) {
            perror("Error al escribir en el archivo de salida");
            close(fd_salida);
            return -6;
        }
    }
    close(fd_salida);

    // Creamos una seríe de contadores para saber cuantos alumnso hay con cada ntoa
    int categoria_M = 0, categoria_S = 0, categoria_N = 0, categoria_A = 0, categoria_F = 0;

    // Recorremos la estructura contabilizando el núemro de alumnos que se encuentra en cada rango de notas
    for (int i = 0; i < num_alumnos; i++) {
        // Usaremos else if ya que no es posible que un alumno se encuentre en varios rangos a la vez
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

    // Una vez hayamos clasificado a los alumnos por sus calificaciones vamos a crear el fichero de estadísticas
    if ((fichero_estadisticas = creat(archivo_estadisticas, 0666)) < 0) {
        perror("Error: no se pudo generar el fichero con las estadísticas");
        return -7;
    }
    
       
    char buffer[100];
    int cadena;
    /*La función snintf, lo que hace es guardarnos en memoria una cadena del tamaño buffer. En esta función, lo primero que debemos pasar es la cadena
    donde se almacenará el resultado, en este caso, buffer. Seguidamente le pasamos el tamaño del buffer, para que sepa cual es el número máximo de 
    caracteres que puede almacenar. Tras esto, le pasamos el formato de la cadena, que este caos será M seguido de un entero y un número en punto flotante,
    que estos será el número de dicha estadisca y el porcentaje de la clase que obtuvo dicha calificación */
    cadena = snprintf(buffer, sizeof(buffer), "M;%d;%.2f%%\n", categoria_M, (categoria_M * 100.0) / num_alumnos);
    if (write(fichero_estadisticas, buffer, cadena) < 0) {
        perror("Error al escribir estadísticas");
        return -8;
    }
    
    cadena = snprintf(buffer, sizeof(buffer), "S;%d;%.2f%%\n", categoria_S, (categoria_S * 100.0) / num_alumnos);
    if (write(fichero_estadisticas, buffer, cadena) < 0) {
        perror("Error al escribir estadísticas");
        return -9;
    }
    
    cadena = snprintf(buffer, sizeof(buffer), "N;%d;%.2f%%\n", categoria_N, (categoria_N * 100.0) / num_alumnos);
    if (write(fichero_estadisticas, buffer, cadena) < 0) {
        perror("Error al escribir estadísticas");
        return -10;
    }
    
    cadena = snprintf(buffer, sizeof(buffer), "A;%d;%.2f%%\n", categoria_A, (categoria_A * 100.0) / num_alumnos);
    if (write(fichero_estadisticas, buffer, cadena) < 0) {
        perror("Error al escribir estadísticas");
        return -11;
    }
    
    cadena = snprintf(buffer, sizeof(buffer), "F;%d;%.2f%%\n", categoria_F, (categoria_F * 100.0) / num_alumnos);
    if (write(fichero_estadisticas, buffer, cadena) < 0) {
        perror("Error al escribir estadísticas");
        return -12;
    }
    
    // Una vez termiando de escribir el fichero estadísticas entonces ya lo podemos cerrar. Asimismo, si hemos llegado a este punto, entonces significa
    // que no hemos encontrado errores en la ejecución del código, por lo que devolvemos -1
    close(fichero_estadisticas);
    return 0; 
}