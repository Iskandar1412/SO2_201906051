// Juan Urbina
// SO2
// 201906051
// gcc HT2_1_201906051.c -lpthread -o HT2_1_201906051

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

/*
void *thread(void *args) {
    printf("Inicio - %s\n", (char*) args);
    sleep(3);
    printf("Termino - %s\n", (char*) args);
}

int main() {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, thread, "Hijo 1");
    pthread_create(&thread2, NULL, thread, "Hijo 2");
    

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
*/


typedef struct {
    long long int inicio;
    long long int fin;
    long long int suma;
} hilos;

void *suma_hilo(void *arg) {
    hilos *hilo = (hilos *) arg;
    long long int total = 0;
    for (long long int i = hilo->inicio; i <= hilo->fin; i++) {
        total += i;
    }
    hilo->suma = total;
    return NULL;
}

int main()  {
    long long int total = 0;
    clock_t start, end;
    double tiempo;
    pthread_t hilo[1];
    hilos hilo1;


    start = clock();

    //vamos con el hijo 1
    hilo1.inicio = 1;
    hilo1.fin = 100000;
    pthread_create(&hilo[0], NULL, suma_hilo, &hilo1);


    //vamos con el hijo 2
    // hilo2.inicio = 50001;
    // hilo2.fin = 100000;
    // pthread_create(&hilo[1], NULL, suma_hilo, &hilo2);

    //esperar a finalizar hilos
    pthread_join(hilo[0], NULL);
    // pthread_join(hilo[1], NULL);

    total = hilo1.suma;

    end = clock();
    tiempo = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecución (1 Hilo): %f (s)\n", tiempo);
    printf("Total: %lld\n", total);

    return 0;
}