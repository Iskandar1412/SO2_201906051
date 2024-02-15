#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

// conversión a archivo .bin
// gcc -o <nombre_archivo>.bin <nombre_archivo>.c

//#define FILEPATH "../File/practica1.txt"
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

    while(1) {
        call = rand() % 3;
        switch (call) {
        case 0: //read file
            printf("Opcion: 0 - ");
            int readed = read(file, palabra, 8);
            if (readed > 0) {
                palabra[readed] = '\0';
                printf("%s\n", palabra);
            } else {
                printf("Error en la lectura\n");
            }
            break;
        case 1: //write file
            printf("Opcion: 1\n");
            for (int i = 0; i < 8; i++) { palabra[i] = rand() % 26 + 'a'; }
            palabra[8] = '\0';
            write(file, palabra, 8);
            break;
        case 2: //seek file
            printf("Opcion: 2\n");
            lseek(file, 0, SEEK_SET);
            break;
        }
        ts.tv_sec = rand() % 3 + 1;
        ts.tv_nsec = 0;
        nanosleep(&ts, NULL);
    }
    close(file);

    return call;
}