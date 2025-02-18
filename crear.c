#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        perror("error in the number of arguments");
        return -1;
    }
    
    char *nombre = argv[1];
    int fd;
    mode_t mode;
    char *endptr;
    
    mode = strtol(argv[2], &endptr, 8);

    mode_t old_mask = umask(0);

    if ((fd = creat(nombre, mode))<0){
        perror("error creating a file");
        return (-1);
    }
    close(fd);
    umask(old_mask);
    return 0;
}

