#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define MAX_RECORDS 1000
#define MAX_LINE_LENGTH 256
#define NUM_THREADS 3
#define NUM_THREADS_OPERATIONS 4


//################ HILOS ################
//################ HILOS ################
//################ HILOS ################
typedef struct {
    FILE *file;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    int current_thread;
    bool finished;
} ThreadData;


ThreadData thread_data;
ThreadData thread_operation;

void ThreadData_init(ThreadData* data, FILE *file) {
    data->file = file;
    pthread_mutex_init(&(data->mutex), NULL);
    pthread_cond_init(&(data->condition), NULL);
    data->current_thread = 0;
    data->finished = false;
}
//################ HILOS ################
//################ HILOS ################
//################ HILOS ################


//############### Usuario ###############
//############### Usuario ###############
//############### Usuario ###############
// Carga de usuario
typedef struct {
    int no_cuenta;
    char nombre[100];
    float saldo;
} Registro;

typedef struct {
    int hilo1;
    int hilo2;
    int hilo3;
} ConteoHilosUsuario;

typedef struct {
    int linea;
    char descripcion[50];
} ErroresUsuarios;

ConteoHilosUsuario hilos_usuarios;
ErroresUsuarios errores_usuarios[100];
int num_errores_registros = 0;
Registro registros[MAX_RECORDS];
int num_registros = 0;
int numero_linea = 0;
// Carga de usuario

// condicional para ver si hay o no hay usuarios repetidos
bool existeCuenta(int numero_cuenta) {
    for (int i = 0; i < num_registros; i++) {
        if (registros[i].no_cuenta == numero_cuenta) {
            return true; 
        }
    }
    return false; 
}

void aumentar_hilo(int hilo) {
    if (hilo == 0) {
        hilos_usuarios.hilo1++;
    } else if (hilo == 1) {
        hilos_usuarios.hilo2++;
    } else if (hilo == 2) {
        hilos_usuarios.hilo3++;
    }
}

bool Cuenta_Positiva(const char *cadena) {
    if (*cadena == '\0')
        return false;
    
    // Verificar si la cadena contiene solo dígitos
    if (strspn(cadena, "0123456789") != strlen(cadena))
        return false;
    
    int numero;
    if (sscanf(cadena, "%d", &numero) != 1) // Intenta convertir la cadena a un entero
        return false;
    
    return numero > 0;
}

bool Saldo_Positivo(const char *cadena) {
    if (*cadena == '\0')
        return false;
    
    double numero;
    if (sscanf(cadena, "%lf", &numero) != 1) // Intenta convertir la cadena a un flotante
        return false;
    
    return numero >= 0.0;
}

void agregarRegistro(int no_cuenta, const char *nombre, float saldo, int hilo) {
    registros[num_registros].no_cuenta = no_cuenta;
    snprintf(registros[num_registros].nombre, sizeof(registros[num_registros].nombre), "%s", nombre);
    registros[num_registros].saldo = saldo;
    aumentar_hilo(hilo);
    num_registros++;
}

// Lectura Archivo usuarios csv
void Analizar_Linea_CSV(char line[], int numero_hilo) {
    numero_linea++;
    // printf("%d - line: %s\n", numero_linea, line);
    // bool primera_linea = true; // declaramos una bandera para no leer la primer linea
    if (numero_linea == 1) {
        return; // Saltar la primera línea
    }

    // Parsear la línea del archivo CSV
    char *token = strtok(line, ",");
    int no_cuenta;
    if (Cuenta_Positiva(token) == false) {
        aumentar_hilo(numero_hilo);
        errores_usuarios[num_errores_registros].linea  = numero_linea;
        strcpy(errores_usuarios[num_errores_registros].descripcion, "No. Cuenta Invalido");
        num_errores_registros++;
        return;
    }
    no_cuenta = atoi(token);

    token = strtok(NULL, ",");
    char nombre[100];
    strcpy(nombre, token);

    token = strtok(NULL, ",");
    float saldo;
    if (Saldo_Positivo(token) == false) {
        aumentar_hilo(numero_hilo);
        errores_usuarios[num_errores_registros].linea  = numero_linea;
        strcpy(errores_usuarios[num_errores_registros].descripcion, "Saldo Invalido");
        num_errores_registros++;
        return;
    }
    saldo = atof(token);
    // Validar los datos
    if (existeCuenta(no_cuenta)) {
        // printf("Error en la línea: %d: Cuenta repetida\n", numero_linea);
        aumentar_hilo(numero_hilo);
        errores_usuarios[num_errores_registros].linea  = numero_linea;
        strcpy(errores_usuarios[num_errores_registros].descripcion, "No. Cuenta Repetida");
        num_errores_registros++;
        return;
    }

    agregarRegistro(no_cuenta, nombre, saldo, numero_hilo);
    
    // Guardar registro
    return;
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
            // printf("Hilo %d: %s", thread_id, line); // 
            Analizar_Linea_CSV(line, thread_id);
        }

        // Se actualiza el hijo para leer
        thread_data.current_thread = (thread_data.current_thread + 1) % NUM_THREADS;

        pthread_cond_broadcast(&(thread_data.condition)); //Envia señal a los hilos
        pthread_mutex_unlock(&(thread_data.mutex)); // Liberar el mutex después de leer

        // Condicional para EOF
        if (thread_data.finished) {
            break;
        }
    }
    pthread_exit(NULL);
}

int CargaUsuarios() {
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
    return 0;
}
//############### Usuario ###############
//############### Usuario ###############
//############### Usuario ###############

//############# Operaciones #############
//############# Operaciones #############
//############# Operaciones #############
// Operaciones
typedef struct {
    int retiros;
    int depositos;
    int transferencias;
} Operacion;

typedef struct {
    int hilo1;
    int hilo2;
    int hilo3;
    int hilo4;
} ConteoHilosOperaciones;

typedef struct {
    int linea;
    char descripcion[50];
} ErrorOperacion;

ConteoHilosOperaciones hilos_operaciones;
Operacion tipo_operacion;
ErrorOperacion errores_operaciones[300];
int num_errores_operaciones = 0;
int num_linea_operacion = 0;

//empiezan funciones de operaciones------------

void AumentarHiloOperaciones(int hilo) {
    if (hilo == 0) {
        hilos_operaciones.hilo1++;
    } else if (hilo == 1) {
        hilos_operaciones.hilo2++;
    } else if (hilo == 2) {
        hilos_operaciones.hilo3++;
    } else if (hilo == 3) {
        hilos_operaciones.hilo4++;
    }
}

void Analizar_Operaciones_CSV(char line[], int numero_hilo) {
    // printf("%s", line);
    num_linea_operacion++;
    if (num_linea_operacion == 1) {
        return;
    }

    char *token = strtok(line, ","); //se obteiene primer valor
    int no_operacion;
    if (Cuenta_Positiva(token) == false) {

    }
}


void* thread_transfers_function(void* args) {
    int thread_id = *((int*)args);
    char line[MAX_LINE_LENGTH];

    while(true) {
        pthread_mutex_lock(&(thread_operation.mutex));
        while(thread_operation.current_thread != thread_id && !thread_operation.finished) {
            pthread_cond_wait(&(thread_operation.condition), &(thread_operation.mutex));
        }

        if (fgets(line, sizeof(line), thread_operation.file) == NULL) {
            thread_operation.finished = true;
        } else {
            Analizar_Operaciones_CSV(line, thread_id);
        }

        thread_operation.current_thread = (thread_operation.current_thread + 1) % NUM_THREADS_OPERATIONS;
        pthread_cond_broadcast(&(thread_operation.condition));
        pthread_mutex_unlock(&(thread_operation.mutex));

        if (thread_operation.finished) {
            break;
        }
    }
    pthread_exit(NULL);
}


void CargaOperaciones() {
    // printf("ASDFASDF");
    FILE *file = fopen("prueba_transacciones.csv", "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return;
    }
    ThreadData_init(&thread_operation, file);
    pthread_t threads[NUM_THREADS_OPERATIONS];
    int thread_ids[NUM_THREADS_OPERATIONS];

    for (int i = 0; i < NUM_THREADS_OPERATIONS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_transfers_function, &thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS_OPERATIONS; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(file);
    return;
}

//############# Operaciones #############
//############# Operaciones #############
//############# Operaciones #############


#include <stdio.h>

int main() {
    int opcion;

    do {
        printf("Menu de Opciones:\n");
        printf("1. Carga Masiva Usuarios\n");
        printf("2. Operaciones Individuales\n");
        printf("3. Carga Masiva Operaciones\n");
        printf("4. Reportes\n");
        printf("5. Salir\n");

        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                printf("Carga Masiva de Usuarios\n");
                CargaUsuarios();
                printf("\n");
                printf("Generando reporte de carga...");
                printf("\n");
                break;
            case 2:
                printf("Operaciones Individuales\n");
                Operaciones_Individuales();
                printf("\n");
                break;
            case 3:
                printf("Carga Masiva Operaciones\n");
                CargaOperaciones();
                printf("\n");
                //funcion carga operaciones
                break;
            case 4:
                printf("Generación de Reportes\n");
                //Función Generar Reportes
                Estado_Cuentas();
                Rep_Carga_Usuarios();
                printf("\n");
                break;
            case 5:
                printf("Saliendo del Programa\n");
                printf("----------------------------------------------\n");
                printf("----------------------------------------------\n");
                printf("-- Created By ------------>  <------------ ---\n");
                printf("-- | | | | |  ----> Juan Urbina [ISKANDAR] ---\n");
                printf("--  | | | | | ---->                1S-2024 ---\n");
                printf("-- | | | | |  ---->                SO2 \"A\" ---\n");
                printf("----------------------------------------------\n");
                printf("----------------------------------------------\n");
                break;
            default:
                printf("Syntax Error\n");
        }

    } while (opcion != 5);

    return 0;
}

void Operaciones_Individuales() {
    int opcion;
    do {
        printf("Menu Operaciones Individuales:\n");
        printf("1. Deposito\n");
        printf("2. Retiro\n");
        printf("3. Transferencia\n");
        printf("4. Salir\n");

        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                printf("Deposito\n");
                //funcion para carga de usuarios
                break;
            case 2:
                printf("Retiro\n");
                // Operaciones_Individuales();
                printf("\n");
                break;
            case 3:
                printf("Transferencia\n");
                //funcion carga operaciones
                break;
            case 4:
                printf("Saliendo del Menu\n");
                break;
            default:
                printf("Syntax Error\n");
        }

    } while (opcion != 4);
    return;
}

//############## Generación Reportes ##############
//############## Generación Reportes ##############
//############## Generación Reportes ##############
//----------Estado de Cuentas
void Estado_Cuentas() {
    FILE *repore_cuentas = fopen("Reportes/Estado_Cuentas.csv", "w");
    if (repore_cuentas == NULL) {
        perror("Creando un reporte");
        exit(EXIT_FAILURE);
    }

    fprintf(repore_cuentas, "no_cuenta,nombre,saldo\n");

    for (int i = 0; i < num_registros; i++) {
        fprintf(repore_cuentas, "%d,%s,%.2f\n", registros[i].no_cuenta, registros[i].nombre, registros[i].saldo);
    }

    fclose(repore_cuentas);
    return;
}

//----------Carga Usuarios
void Rep_Carga_Usuarios() {
    time_t current_time;
    struct tm *local_time;
    char tiempo_formateado [46];
    char tiempo_2 [30];

    current_time = time(NULL);
    local_time = localtime(&current_time);

    // char file_name [50];
    strftime(tiempo_formateado, 46, "Reportes/Cargas/carga_%Y_%m_%d-%H_%M_%S.log", local_time);//19
    strftime(tiempo_2, 20, "%Y-%m-%d %H:%M:%S", local_time);//19
    // printf("Fecha %s\n", tiempo_formateado);

    // snprintf(file_name + strlen(file_name), sizeof(file_name) - strlen(file_name), ".log");
    // snprintf(file_name + strlen(directorio), sizeof(file_name) - strlen(directorio), "%s", file_name);

    // printf("%s", tiempo_formateado);
    FILE *reporte_usuarios = fopen(tiempo_formateado, "w");

    fprintf(reporte_usuarios, "--------------------- Carga de Usuarios ---------------------\n");
    fprintf(reporte_usuarios, tiempo_2);
    fprintf(reporte_usuarios, "\n");
    fprintf(reporte_usuarios, "Usuarios Cargados: \n");
    fprintf(reporte_usuarios, "Hilo #1: %d\n", hilos_usuarios.hilo1);
    fprintf(reporte_usuarios, "Hilo #2: %d\n", hilos_usuarios.hilo2);
    fprintf(reporte_usuarios, "Hilo #3: %d\n", hilos_usuarios.hilo3);
    fprintf(reporte_usuarios, "Total: %d\n", (hilos_usuarios.hilo1 + hilos_usuarios.hilo2 + hilos_usuarios.hilo3));
    fprintf(reporte_usuarios, "\n");
    fprintf(reporte_usuarios, "Errores: \n");
    for (int i = 0; i < num_errores_registros; i++) {
        fprintf(reporte_usuarios, " - Linea #%d: %s\n", errores_usuarios[i].linea, errores_usuarios[i].descripcion);
    }
    fprintf(reporte_usuarios, "-------------------------------------------------------------\n");
    fclose(reporte_usuarios);
    return;
}
