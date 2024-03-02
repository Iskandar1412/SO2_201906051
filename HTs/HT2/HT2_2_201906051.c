// Juan Urbina
// SO2
// gcc HT2_2_201906051.c -lpthread -o HT2_2_201906051

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

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
    pthread_t hilo[2];
    hilos hilo1, hilo2;


    start = clock();

    //vamos con el hijo 1
    hilo1.inicio = 1;
    hilo1.fin = 50000;
    pthread_create(&hilo[0], NULL, suma_hilo, &hilo1);


    //vamos con el hijo 2
    hilo2.inicio = 50001;
    hilo2.fin = 100000;
    pthread_create(&hilo[1], NULL, suma_hilo, &hilo2);

    //esperar a finalizar hilos
    pthread_join(hilo[0], NULL);
    pthread_join(hilo[1], NULL);

    total = hilo1.suma + hilo2.suma;

    end = clock();
    tiempo = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecución (2 Hilos): %f (s)\n", tiempo);
    printf("Total: %lld\n", total);

    return 0;
}