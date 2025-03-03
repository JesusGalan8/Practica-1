#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ALUMNOS 100

struct alumno {
    char nombre[50];
    int nota;
    int convocatoria;
};
typedef struct alumno tAlumno;

const char *nombres[] = {
    "Alejandro", "Beatriz", "Carlos", "Diana", "Eduardo", "Fernanda", "Gonzalo", "Hector", "Isabel", "Javier",
    "Karla", "Luis", "Marta", "Natalia", "Oscar", "Patricia", "Quentin", "Raquel", "Sergio", "Teresa",
    "Ulises", "Valeria", "Walter", "Ximena", "Yolanda", "Zacarías", "Andrea", "Bruno", "Claudia", "David",
    "Elena", "Francisco", "Gabriela", "Hugo", "Ivonne", "José", "Karen", "Leonardo", "Monica", "Nicolás",
    "Olga", "Pablo", "Queralt", "Rafael", "Sofía", "Tomás", "Urbano", "Vanessa", "William", "Xavier",
    "Yara", "Zulema", "Adrián", "Bárbara", "Cristina", "Diego", "Esteban", "Felipe", "Gerardo", "Helena",
    "Ignacio", "Juliana", "Kevin", "Laura", "Mauricio", "Norma", "Orlando", "Paola", "Quirino", "Ricardo",
    "Susana", "Tadeo", "Ursula", "Victor", "Wilfredo", "Xenia", "Yago", "Zenaida", "Álvaro", "Bianca",
    "César", "Daniela", "Emilio", "Fabiola", "Guillermo", "Horacio", "Iván", "Jacqueline", "Kurt", "Lucía",
    "Manuel", "Nerea", "Omar", "Priscila", "Quique", "Rodrigo", "Selena", "Teodoro", "Ubaldo", "Verónica"
};

int main() {
    FILE *f = fopen("alumnos.dat", "wb");
    if (!f) {
        perror("Error al abrir el archivo");
        return 1;
    }

    srand(time(NULL));
    tAlumno alumnos[MAX_ALUMNOS];

    for (int i = 0; i < MAX_ALUMNOS; i++) {
        snprintf(alumnos[i].nombre, 50, "%s", nombres[i]);
        alumnos[i].nota = rand() % 11;  // Notas de 0 a 10
        alumnos[i].convocatoria = rand() % 3 + 1;  // Convocatoria entre 1 y 3
    }

    fwrite(alumnos, sizeof(tAlumno), MAX_ALUMNOS, f);
    fclose(f);
    
    printf("Archivo 'alumnos.dat' generado correctamente.\n");
    return 0;
}
