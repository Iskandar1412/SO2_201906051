#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 3
#define MAX_LINE_LENGTH 256

typedef struct {
    int hilo1;
    int hilo2;
    int hilo3;
} ConteoHilosUsuario;

ConteoHilosUsuario hilos_usuarios;

typedef struct {
    FILE *file;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    int current_thread;
    bool finished;
} ThreadData;

ThreadData thread_data;

void ThreadData_init(ThreadData* data, FILE *file) {
    data->file = file;
    pthread_mutex_init(&(data->mutex), NULL);
    pthread_cond_init(&(data->condition), NULL);
    data->current_thread = 0;
    data->finished = false;
}

void* thread_users_function(void* args) {
    int thread_id = *((int*)args); // Obtención del hijo (numero)
    char line[MAX_LINE_LENGTH];

    // Bloquear el mutex para la lectura
    while (true) {
        pthread_mutex_lock(&(thread_data.mutex)); 

        // Wait para que los hijos no se metan a lo loco
        while (thread_data.current_thread != thread_id && !thread_data.finished) {
            pthread_cond_wait(&(thread_data.condition), &(thread_data.mutex));
        }

        // Lectura de linea por hijo
        if (fgets(line, sizeof(line), thread_data.file) == NULL) {
            thread_data.finished = true;
        } else {
            printf("Hilo %d: %s", thread_id, line); // Mostrar el número de hilo y la línea leída
            if (thread_id == 0) {
                hilos_usuarios.hilo1++;
            } else if (thread_id == 1) {
                hilos_usuarios.hilo2++;
            } else if (thread_id == 2) {
                hilos_usuarios.hilo3++;
            }
        }

        // Se actualiza el hijo para leer
        thread_data.current_thread = (thread_data.current_thread + 1) % NUM_THREADS;

        pthread_cond_broadcast(&(thread_data.condition)); // Avisar a todos los hilos que se ha leído una línea
        pthread_mutex_unlock(&(thread_data.mutex)); // Liberar el mutex después de leer

        // Condicional para EOF
        if (thread_data.finished) {
            break; 
        }
    }

    pthread_exit(NULL);
}

int main() {
    FILE *file = fopen("prueba_usuarios.csv", "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return EXIT_FAILURE;
    }

    ThreadData_init(&thread_data, file);

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i; // Numero de hijo (3)
        pthread_create(&threads[i], NULL, thread_users_function, &thread_ids[i]);
    }

    // Esperar la finalización de los hijos
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(file);

    printf("1: %d, 2: %d, 3: %d\n", hilos_usuarios.hilo1, hilos_usuarios.hilo2, hilos_usuarios.hilo3);
    return EXIT_SUCCESS;
}
