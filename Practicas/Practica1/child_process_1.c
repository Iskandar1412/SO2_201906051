#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define FILEPATH "../File/practica1.txt"

int valor() {
    int valor = rand() % 36;
    if (valor < 10) {
        return '0' + valor;
    } else {
        return 'a' + valor - 10;
    }
}

void read_file(FILE *archivo) {
    char line[8];
    fread(line, sizeof(line), 0, archivo);
    line[8] = '\0';
    printf("Lectura archivo %s\n", line);
}

void escritura_archivo(FILE *archivo)  {
    char line[9];
    for (int i = 0; i < 8; i++) {
        line[i] = valor();
        //printf("%zu\n", line[i]);
    }
    line[8] = '\0';
    fprintf(archivo, "%s\n", line);
}

int main() {
    FILE *file = fopen(FILEPATH, "w+");
    if (file == NULL) {
        perror("Error to open the file");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL));

    while(1) {
        int llamada = rand() % 3;
        int time = rand() % 3 + 1;
        switch(llamada) {
            case 0:
                escritura_archivo(file);
                break;
            case 1:
                read_file(file);
                break;
            case 2:
                fseek(file, 0, SEEK_SET);
                break;
        }
        usleep(time * 1000000);
    }
    
    fclose(file);
    return 0;
}