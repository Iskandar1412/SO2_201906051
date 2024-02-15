#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define FILEPATH "./File/practica1.txt"

int main() {
    int file, call;
    char palabra[8];
    struct timespec ts;

    file = open(FILEPATH, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (file < 0) {
        perror("Error al abrir archivo");
        exit(1);
    }

    while(1)

    return 0;
}