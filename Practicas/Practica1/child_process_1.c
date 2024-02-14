#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define FILEPATH "./File/practica1.txt"

int valor() {
    return rand() % 26;
}

void escritura_archivo(FILE *archivo)  {
    char line[8];
    for (int i = 0; i < sizeof(line); i++) {
        line[i] = 'a' + valor();
        printf("valor %zu\n", line[i]);

    }   
}

int main() {
    FILE *file = fopen(FILEPATH, "w+");
    if (file == NULL) {
        perror("Error to open the file");
        exit(EXIT_FAILURE);
    }
    escritura_archivo(file);
    fclose(file);
    return 0;
}