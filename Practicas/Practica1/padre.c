#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

//variables globales
int llamadas_tot = 0;
int contador_llamadas[3] = {0};

//ctrl + c
void signal_SIGINT() {
    int i;
}

int main() {
    return 0;
}