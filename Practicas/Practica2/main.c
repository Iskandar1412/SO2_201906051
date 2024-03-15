#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define MAX_LINEAS 1000
#define MAX_ERRORES_USUARIO 1000
#define MAX_USUARIOS 1000
#define MAX_ERRORES_OPERACION 1000
#define MAX_OPERACIONES 1000

// Carga de usuario
typedef struct {
    int hilo1, hilo2,hilo3;
    char usuario[30];
    float balance;
}Usuario;

typedef  struct {
    int linea;
    char descripcion[50];
}ErrorUsuario;

// Operaciones
typedef struct {
    int hilo1, hilo2, hilo3, hilo4;
    int retiros;
    int depositos;
    int transferencias;
}Operacion;

typedef struct {
    int linea;
    char descripcion[50];
}ErrorOperacion;


Usuario usuarios[MAX_USUARIOS];
ErrorUsuario err_users[MAX_ERRORES_USUARIO];
Operacion operaciones[MAX_OPERACIONES];
ErrorOperacion err_operaciones[MAX_OPERACIONES];


void* load_users(void* arg) {
    FILE* file = fopen("prueba_usuarios.csv", "r");
    if (!file) {
        perror("Error al abrir el archivo de carga de usuarios");
        pthread_exit(NULL);
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        // Lectura archivo
        int account_number;
        char name[100];
        float balance;
        if (sscanf(buffer, "%d,%99[^,],%f", &account_number, name, &balance) == 3) {
            // Carga de usuario
            printf("[hilo %d]:Cargando usuario: %d, %s, %.2f\n", account_number, name, balance);
        } else {
            // Errores
            printf("Error en el formato de la línea: %s\n", buffer);
        }
    }

    fclose(file);
    pthread_exit(NULL);
}


void* load_operations(void* arg) {
    FILE* file = fopen("prueba_transacciones.csv", "r");
    if (!file) {
        perror("Error al abrir el archivo de carga de operaciones");
        pthread_exit(NULL);
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        // Carga csv
        int operation_type, account1, account2;
        float amount;
        if (sscanf(buffer, "%d,%d,%d,%f", &operation_type, &account1, &account2, &amount) == 4) {
            // Carga de operaciones
            printf("Cargando operación: Tipo %d, Cuenta1 %d, Cuenta2 %d, Monto %.2f\n", operation_type, account1, account2, amount);
        } else {
            // Errores
            printf("Error en el formato de la línea: %s\n", buffer);
        }
    }

    fclose(file);
    pthread_exit(NULL);
}


int main() {
    // Declarar hilos
    pthread_t user_threads[3];
    pthread_t operation_threads[4];

    // Crear hilos para cargar usuarios
    for (int i = 0; i < 3; i++) {
        pthread_create(&user_threads[i], NULL, load_users, NULL);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(user_threads[i], NULL);
    }

    // Crear hilos para cargar operaciones
    for (int i = 0; i < 4; i++) {
        pthread_create(&operation_threads[i], NULL, load_operations, NULL);
    }

    // Esperar a que los hilos terminen

    for (int i = 0; i < 4; i++) {
        pthread_join(operation_threads[i], NULL);
    }
    // Realizar las operaciones necesarias y generar los reportes
    // deposit(12345, 100.0);
    // withdraw(54321, 50.0);
    // transfer(12345, 54321, 25.0);
    // check_balance(12345);
    // generate_accounts_report();
    // generate_users_load_report();
    // generate_operations_load_report();

    return 0;
}
