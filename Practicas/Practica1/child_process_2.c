#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

// conversión a archivo .bin
// gcc -o <nombre_archivo>.bin <nombre_archivo>.c
// gcc -o child_process_1.bin child_process_1.c

int total_calls = 0;
int read_calls = 0;
int write_calls = 0;
int seek_calls = 0;

#define FILEPATH "../File/practica1.txt"

volatile sig_atomic_t sigint_received = 0;

void sigint_handler(int sig) {
    sigint_received = 1;
}

void sigterm_handler(int sig) {
    printf("El proceso hijo se ha cerrado\n");
    exit(0);
}

int main() {
    int file, call;
    char palabra[8];
    struct timespec ts;

    srand(time(NULL));
    signal(SIGINT, sigint_handler);

    file = open(FILEPATH, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (file < 0) {
        perror("[Child] Error al abrir archivo");
        exit(EXIT_FAILURE);
    }

    while(!sigint_received) {
        call = rand() % 3;
        switch (call) {
        case 0: //read file
            //printf("Opcion: 0\n");
            int readed = read(file, palabra, 8);
            //if (readed > 0) {
            //    palabra[readed] = '\0';
            //    printf("%s\n", palabra);
            //} else {
            //    printf("Error en la lectura\n");
            //}
            total_calls++;
            read_calls++;
            break;
        case 1: //write file
            //printf("Opcion: 1\n");
            for (int i = 0; i < 8; i++) { palabra[i] = rand() % 26 + 'a'; }
            palabra[8] = '\0';
            write(file, palabra, 8);
            total_calls++;
            write_calls++;
            break;
        case 2: //seek file
            //printf("Opcion: 2\n");
            lseek(file, 0, SEEK_SET);
            total_calls++;
            seek_calls++;
            break;
        }
        ts.tv_sec = rand() % 3 + 1;
        ts.tv_nsec = 0;
        nanosleep(&ts, NULL);
    }

    printf("Total Llamadas [Child 2]: %d\n\tRead: %d - Write: %d - Seek: %d\n", total_calls, read_calls, write_calls, seek_calls);
    close(file);

    return 0;
}
