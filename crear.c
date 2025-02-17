#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>


int main (char *nombre, int mode){
    int fd;

    mode_t old_mask = umask(0);

    if (fd = creat(nombre, mode)<0){
        perror("error creating a file");
        return (-1);
    }
    close(fd);
    umask(old_mask);
    return 0;
}

