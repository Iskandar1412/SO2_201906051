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

bool Cuenta_2_Positiva(const char *cadena) {
    if (*cadena == '\0')
        return false;
    
    // Verificar si la cadena contiene solo dígitos
    if (strspn(cadena, "0123456789") != strlen(cadena))
        return false;
    
    int numero;
    if (sscanf(cadena, "%d", &numero) != 1) // Intenta convertir la cadena a un entero
        return false;
    
    return numero >= 0;
}

bool Saldo_Positivo(const char *cadena) {
    if (*cadena == '\0')
        return false;
    
    double numero;
    if (sscanf(cadena, "%lf", &numero) != 1) // Intenta convertir la cadena a un flotante
        return false;
    
    return numero > 0.0;
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
    FILE *file = fopen("files/prueba_usuarios.csv", "r");
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
    int Desconocidos;
} Operacion;

typedef struct {
    int hilo1;
    int hilo2;
    int hilo3;
    int hilo4;
} ConteoHilosOperaciones;

typedef struct {
    int linea;
    char descripcion[100];
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

void Deposito(char line[], int numero_hilo) {
    tipo_operacion.depositos ++;
    char *token = strtok(line, ","); //tipo 1
    token = strtok(NULL, ","); //cuenta
    int cuenta_1;
    if (Cuenta_Positiva(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Deposito]: Cuenta 1 no Valida");
        num_errores_operaciones++;
        return;
    }
    cuenta_1 = atoi(token);
    // printf("[primero pasado %s\n", token);
    token = strtok(NULL, ","); //cuenta 2
    // printf("%s\n", token);
    int cuenta_2;
    if (Cuenta_2_Positiva(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Deposito]: Cuenta 2 no Valida");
        num_errores_operaciones++;
        return;
    }
    cuenta_2 = atoi(token);
    // printf("segundo pasado \n");
    if (cuenta_2 != 0) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Deposito]: No debe haber una segunda cuenta en depositos");
        num_errores_operaciones++;
        return;
    }
    //monto
    token = strtok(NULL, ",");
    // printf("segundo Cuenta %d\n", token);
    float monto;
    if (Saldo_Positivo(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Deposito]: Monto no Valido");
        num_errores_operaciones++;
        return;
    }
    monto = atof(token);
    // printf("tercero pasado] %.2f\n", monto);
    if (existeCuenta(cuenta_1) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Deposito]: No existe la cuenta 1");
        num_errores_operaciones++;
        return;
    }
    // printf("final %d - %.2f\n", cuenta_1, monto);
    for (int i = 0; i < num_registros; i++) {
        if (registros[i].no_cuenta == cuenta_1) {
            // printf("cuenta: %d existente, monto: %.2f\n", cuenta_1, monto);
            registros[i].saldo += monto;
            AumentarHiloOperaciones(numero_hilo);
            return;
        }
    }
}

void Retiro(char line[], int numero_hilo) {
    tipo_operacion.retiros ++;
    char *token = strtok(line, ","); //tipo 1
    token = strtok(NULL, ","); //cuenta
    int cuenta_1;
    if (Cuenta_Positiva(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: Cuenta 1 no Valida");
        num_errores_operaciones++;
        return;
    }
    cuenta_1 = atoi(token);
    // printf("[primero pasado %s\n", token);
    token = strtok(NULL, ","); //cuenta 2
    // printf("%s\n", token);
    int cuenta_2;
    if (Cuenta_2_Positiva(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: Cuenta 2 no Valida");
        num_errores_operaciones++;
        return;
    }
    cuenta_2 = atoi(token);
    // printf("segundo pasado \n");
    if (cuenta_2 != 0) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: No debe haber una segunda cuenta en depositos");
        num_errores_operaciones++;
        return;
    }
    //monto
    token = strtok(NULL, ",");
    // printf("segundo Cuenta %d\n", token);
    float monto;
    if (Saldo_Positivo(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: Monto no Valido");
        num_errores_operaciones++;
        return;
    }
    monto = atof(token);
    // printf("tercero pasado] %.2f\n", monto);
    if (existeCuenta(cuenta_1) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: No existe la cuenta 1");
        num_errores_operaciones++;
        return;
    }
    // printf("final %d - %.2f\n", cuenta_1, monto);
    for (int i = 0; i < num_registros; i++) {
        if (registros[i].no_cuenta == cuenta_1) {
            // printf("saldo anterior: %.2f\n", registros[i].saldo);
            if (registros[i].saldo < monto) {
                // printf("Saldo insuficiente %.2f\n", monto);
                AumentarHiloOperaciones(numero_hilo);
                errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
                strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: Saldo insuficiente");
                num_errores_operaciones++;
                return;
            }
            registros[i].saldo -= monto;
            // printf("saldo actual: %.2f\n", registros[i].saldo);
            AumentarHiloOperaciones(numero_hilo);
            return;
        }
    }
}

void Transferencia(char line[], int numero_hilo) {
    tipo_operacion.transferencias++;
    char *token = strtok(line, ","); //tipo 1
    token = strtok(NULL, ","); //cuenta
    int cuenta_1;
    if (Cuenta_Positiva(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: Cuenta 1 no Valida");
        num_errores_operaciones++;
        return;
    }
    cuenta_1 = atoi(token);
    // printf("[primero pasado %s\n", token);
    token = strtok(NULL, ","); //cuenta 2
    // printf("%s\n", token);
    int cuenta_2;
    if (Cuenta_2_Positiva(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: Cuenta 2 no Valida");
        num_errores_operaciones++;
        return;
    }
    cuenta_2 = atoi(token);
    //monto
    token = strtok(NULL, ",");
    // printf("segundo Cuenta %d\n", token);
    float monto;
    if (Saldo_Positivo(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: Monto no Valido");
        num_errores_operaciones++;
        return;
    }
    monto = atof(token);
    // printf("tercero pasado] %.2f\n", monto);
    if (existeCuenta(cuenta_1) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: No existe la cuenta 1");
        num_errores_operaciones++;
        return;
    }

    if (existeCuenta(cuenta_2) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: No existe la cuenta 2");
        num_errores_operaciones++;
        return;
    }

    // printf("final %d - %.2f\n", cuenta_1, monto);
    for (int i = 0; i < num_registros; i++) {
        for (int j = 0; j < num_registros; j++) {
            if ((registros[i].no_cuenta == cuenta_1) && (registros[j].no_cuenta == cuenta_2)) {
                // printf("Cuenta 1: %d - Cuenta 2: %d\n", cuenta_1, cuenta_2);
                // printf("saldo anterior 1: %.2f\n", registros[i].saldo);
                // printf("saldo anterior 2: %.2f\n", registros[j].saldo);
                // printf("Cuenta 1R: %d - Cuenta 2R: %d\n", registros[i].no_cuenta, registros[j].no_cuenta);
                if (registros[i].saldo < monto) {
                    // printf("Saldo insuficiente %.2f\n", monto);
                    AumentarHiloOperaciones(numero_hilo);
                    errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
                    strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: Saldo insuficiente");
                    num_errores_operaciones++;
                    return;
                }
                registros[i].saldo -= monto;
                registros[j].saldo += monto;
                // printf("saldo actual 1: %.2f\n", registros[i].saldo);
                // printf("saldo actual 2: %.2f\n", registros[j].saldo);
                AumentarHiloOperaciones(numero_hilo);
                // tipo_operacion.transferencias ++;
                return;
            }
        }
    }
}

void Analizar_Operaciones_CSV(char line[], int numero_hilo) {
    // printf("%s", line);
    num_linea_operacion++;
    if (num_linea_operacion == 1) {
        return;
    }

    char valor[MAX_LINE_LENGTH];
    strcpy(valor, line);
    // printf("%s--------\n", valor);
    char *token = strtok(line, ","); //se obteiene primer valor
    int tipo_op;
    if (Cuenta_Positiva(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "Operacion no Valida");
        num_errores_operaciones++;
        return;
    }
    tipo_op = atoi(token);

    if (tipo_op == 1) { 
        //deposito cuenta1 (cuenta a depositar), cuenta2 (vacia), monto
        Deposito(valor, numero_hilo);
        // printf("DEPOSITO\n");
    } else if (tipo_op == 2) { 
        //retiro cuenta1(cuenta a retirar), cuenta2 (vacio), monto a retirar
        Retiro(valor, numero_hilo);
        // printf("Retiro\n");
    } else if (tipo_op == 3) { 
        //transferencia cuenta1 (cuenta de la que se retira), cuenta2 (cuenta a la que se deposita), monto (monto a depositar de cuenta1 a cuenta2)
        Transferencia(valor, numero_hilo);
        // printf("TRANSFERENCIA\n");
    } else {
        //Error de operacion
        tipo_operacion.Desconocidos ++;
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "Operacion Invalida");
        num_errores_operaciones++;
        return;
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
    FILE *file = fopen("files/prueba_transacciones.csv", "r");
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

//####### Operaciones Individuales #######
//####### Operaciones Individuales #######
//####### Operaciones Individuales #######

void Deposito_Individual() {
    num_linea_operacion++;
    char e_cuenta1[25], e_monto[25];
    int cuenta1;
    float monto;

    printf("Ingresar cuenta: ");
    scanf("%s", &e_cuenta1);
    if (Cuenta_2_Positiva(e_cuenta1) == false) {
        printf("Error al ingresar cuenta\n");
        errores_operaciones[num_errores_operaciones].linea = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Deposito]: Cuenta no Valida");
        num_errores_operaciones++;
        return;
    }
    cuenta1 = atoi(e_cuenta1);
    // printf("Ingresar monto: %d\n", cuenta1);
    printf("Ingresar monto a depositar: ");
    scanf("%s", &e_monto);
    if (Saldo_Positivo(e_monto) == false) {
        printf("Error de monto\n");
        errores_operaciones[num_errores_operaciones].linea = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Deposito]: Saldo no valido");
        num_errores_operaciones++;
        return;
    }
    monto = atof(e_monto);

    if (existeCuenta(cuenta1) == false) {
        printf("Error cuenta no existente\n");
        // AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Deposito]: No existe la cuenta");
        num_errores_operaciones++;
        return;
    }

    for (int i = 0; i<num_registros; i++) {
        if (registros[i].no_cuenta == cuenta1) {
            registros[i].saldo += monto;
            // AumentarHiloOperaciones(numero_hilo);
            // tipo_operacion.depositos ++;
            return;
        }
    }
}

void Retiro_Individual() {
    num_linea_operacion++;
    char e_cuenta1[25], e_monto[25];
    int cuenta1;
    float monto;

    printf("Ingresar cuenta: ");
    scanf("%s", &e_cuenta1);
    if (Cuenta_2_Positiva(e_cuenta1) == false) {
        printf("Error en ingresar cuenta\n");
        errores_operaciones[num_errores_operaciones].linea = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: Cuenta no Valida");
        num_errores_operaciones++;
        return;
    }
    cuenta1 = atoi(e_cuenta1);
    // printf("Ingresar monto: %d\n", cuenta1);
    printf("Ingresar monto a retirar: ");
    scanf("%s", &e_monto);
    if (Saldo_Positivo(e_monto) == false) {
        printf("Error en el monto\n");
        errores_operaciones[num_errores_operaciones].linea = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: Saldo no valido");
        num_errores_operaciones++;
        return;
    }
    monto = atof(e_monto);

    if (existeCuenta(cuenta1) == false) {
        printf("Cuenta no existente\n");
        // AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: No existe la cuenta");
        num_errores_operaciones++;
        return;
    }

    for (int i = 0; i<num_registros; i++) {
        if (registros[i].no_cuenta == cuenta1) {
            if (registros[i].saldo < monto) {
                printf("Error de monto\n");
                // AumentarHiloOperaciones(numero_hilo);
                errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
                strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: Saldo insuficiente");
                num_errores_operaciones++;
                return;
            }
            registros[i].saldo -= monto;
            // AumentarHiloOperaciones(numero_hilo);
            // tipo_operacion.depositos ++;
            return;
        }
    }
}

void Transferencia_Individual() {
    num_linea_operacion++;
    char e_cuenta1[25], e_cuenta2[25], e_monto[25];
    int cuenta1, cuenta2;
    float monto;

    printf("Ingresar cuenta (sacar): ");
    scanf("%s", &e_cuenta1);
    if (Cuenta_2_Positiva(e_cuenta1) == false) {
        printf("Error de cuenta\n");
        errores_operaciones[num_errores_operaciones].linea = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: Cuenta no Valida");
        num_errores_operaciones++;
        return;
    }
    cuenta1 = atoi(e_cuenta1);

    printf("Ingresar cuenta (Transferir): ");
    scanf("%s", &e_cuenta2);
    if (Cuenta_2_Positiva(e_cuenta2) == false) {
        printf("Error de cuenta\n");
        errores_operaciones[num_errores_operaciones].linea = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: Cuenta no Valida");
        num_errores_operaciones++;
        return;
    }
    cuenta2 = atoi(e_cuenta2);

    // printf("Ingresar monto: %d\n", cuenta1);
    printf("Ingresar monto a debitar: ");
    scanf("%s", &e_monto);
    if (Saldo_Positivo(e_monto) == false) {
        printf("Error en el saldo\n");
        errores_operaciones[num_errores_operaciones].linea = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: Saldo no valido");
        num_errores_operaciones++;
        return;
    }
    monto = atof(e_monto);

    if (existeCuenta(cuenta1) == false) {
        printf("Cuenta 1 no existente\n");
        // AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: No existe la cuenta");
        num_errores_operaciones++;
        return;
    }

    if (existeCuenta(cuenta2) == false) {
        printf("Cuenta 2 no existente\n");
        // AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: No existe la cuenta 2");
        num_errores_operaciones++;
        return;
    }

    for (int i = 0; i < num_registros; i++) {
        for (int j = 0; j < num_registros; j++) {
            if ((registros[i].no_cuenta == cuenta1) && (registros[j].no_cuenta == cuenta2)) {
                if (registros[i].saldo < monto) {
                    printf("Error de monto\n");
                    // AumentarHiloOperaciones(numero_hilo);
                    errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
                    strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: Saldo insuficiente");
                    num_errores_operaciones++;
                    return;
                }
                registros[j].saldo += monto;
                registros[i].saldo -= monto;
                // AumentarHiloOperaciones(numero_hilo);
                // tipo_operacion.depositos ++;
                return;
            }
        }
    }
}

void Ver_Cuenta() {
    // num_linea_operacion++;
    char e_cuenta1[25];
    int cuenta1;

    printf("Ingresar cuenta a consultar: ");
    scanf("%s", &e_cuenta1);
    if (Cuenta_2_Positiva(e_cuenta1) == false) {
        printf("Error de cuenta\n");
        errores_operaciones[num_errores_operaciones].linea = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: Cuenta no Valida");
        num_errores_operaciones++;
        return;
    }
    cuenta1 = atoi(e_cuenta1);

    if (existeCuenta(cuenta1) == false) {
        printf("Cuenta no existente\n");
        // AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: No existe la cuenta");
        num_errores_operaciones++;
        return;
    }

    for (int i = 0; i < num_registros; i++) {
        if (registros[i].no_cuenta == cuenta1) {
            printf("Información Cuenta ----------\n");
            printf("No: %d\n", registros[i].no_cuenta);
            printf("Nombre: %s\n", registros[i].nombre);
            printf("Saldo: %.2f\n", registros[i].saldo);
            printf("Información Cuenta ----------\n");
            return;
        }
    }
}

//####### Operaciones Individuales #######
//####### Operaciones Individuales #######
//####### Operaciones Individuales #######
//----------------------------------------
//################# MENU #################
//################# MENU #################
//################# MENU #################

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
                printf("Generando reporte de carga...");
                printf("\n");
                Rep_Carga_Usuarios();
                printf("\n");
                break;
            case 2:
                printf("Operaciones Individuales\n");
                Operaciones_Individuales();
                printf("\n");
                break;
            case 3:
                printf("Carga Masiva Operaciones...\n");
                CargaOperaciones();
                printf("Generando reporte de carga...");
                printf("\n");
                Rep_Carga_Operaciones();
                printf("\n");
                //funcion carga operaciones
                break;
            case 4:
                printf("Generando Reportes...\n");
                //Función Generar Reportes
                Estado_Cuentas();
                // Rep_Carga_Usuarios();
                // Rep_Carga_Operaciones();
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
        printf("4. Consulta de Cuenta\n");
        printf("5. Salir\n");

        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                printf("Deposito\n");
                //funcion para carga de usuarios
                Deposito_Individual();
                printf("\n");
                break;
            case 2:
                printf("Retiro\n");
                // Operaciones_Individuales();
                Retiro_Individual();
                printf("\n");
                break;
            case 3:
                printf("Transferencia\n");
                Transferencia_Individual();
                printf("\n");
                //funcion carga operaciones
                break;
            case 4:
                printf("Consulta Cuenta\n");
                Ver_Cuenta();
                printf("\n");
                //funcion carga operaciones
                break;
            case 5:
                printf("Saliendo del Menu (Operaciones Individuales)\n");
                break;
            default:
                printf("Syntax Error\n");
        }

    } while (opcion != 5);
    return;
}

//################# MENU #################
//################# MENU #################
//################# MENU #################

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
        fprintf(reporte_usuarios, " - Linea #%d:\t%s\n", errores_usuarios[i].linea, errores_usuarios[i].descripcion);
    }
    fprintf(reporte_usuarios, "-------------------------------------------------------------\n");
    fclose(reporte_usuarios);
    return;
}


void Rep_Carga_Operaciones() {
    time_t current_time;
    struct tm *local_time;
    char tiempo_formateado [58];
    char tiempo_2 [30];

    current_time = time(NULL);
    local_time = localtime(&current_time);

    // char file_name [50];                   Operaciones       operaciones
    strftime(tiempo_formateado, 58, "Reportes/Operaciones/operaciones_%Y_%m_%d-%H_%M_%S.log", local_time);//19
    strftime(tiempo_2, 20, "%Y-%m-%d %H:%M:%S", local_time);//19
    // printf("Fecha %s\n", tiempo_formateado);

    // snprintf(file_name + strlen(file_name), sizeof(file_name) - strlen(file_name), ".log");
    // snprintf(file_name + strlen(directorio), sizeof(file_name) - strlen(directorio), "%s", file_name);

    // printf("%s", tiempo_formateado);
    FILE *reporte_usuarios = fopen(tiempo_formateado, "w");

    // fprintf(reporte_usuarios, "--------------------- Carga de Usuarios ---------------------\n");
    fprintf(reporte_usuarios, "-------------------- Carga de Operaciones --------------------\n");
    fprintf(reporte_usuarios, tiempo_2);
    fprintf(reporte_usuarios, "\n");
    fprintf(reporte_usuarios, "Operaciones Realizadas: \n");
    fprintf(reporte_usuarios, "Retiros: %d\n", tipo_operacion.retiros);
    fprintf(reporte_usuarios, "Depositos: %d\n", tipo_operacion.depositos);
    fprintf(reporte_usuarios, "Transferencias: %d\n", tipo_operacion.transferencias);
    fprintf(reporte_usuarios, "Desconocidos: %d\n", tipo_operacion.Desconocidos);
    fprintf(reporte_usuarios, "Total: %d\n", (tipo_operacion.depositos + tipo_operacion.retiros + tipo_operacion.transferencias + tipo_operacion.Desconocidos));
    fprintf(reporte_usuarios, "\n");
    fprintf(reporte_usuarios, "Operaciones por Hilo: \n");
    fprintf(reporte_usuarios, "Hilo #1: %d\n", hilos_operaciones.hilo1);
    fprintf(reporte_usuarios, "Hilo #2: %d\n", hilos_operaciones.hilo2);
    fprintf(reporte_usuarios, "Hilo #3: %d\n", hilos_operaciones.hilo3);
    fprintf(reporte_usuarios, "Hilo #4: %d\n", hilos_operaciones.hilo4);
    fprintf(reporte_usuarios, "Total: %d\n", (hilos_operaciones.hilo1 + hilos_operaciones.hilo2 + hilos_operaciones.hilo3 + hilos_operaciones.hilo4));
    fprintf(reporte_usuarios, "\n");
    fprintf(reporte_usuarios, "Errores: \n");
    for (int i = 0; i < num_errores_operaciones; i++) {
        // printf("%d\n", errores_operaciones[i].linea);
        fprintf(reporte_usuarios, " - Linea #%d:\t%s\n", errores_operaciones[i].linea, errores_operaciones[i].descripcion);
    }
    fprintf(reporte_usuarios, "--------------------------------------------------------------\n");
    fclose(reporte_usuarios);
    return;
}

//############## Generación Reportes ##############
//############## Generación Reportes ##############
//############## Generación Reportes ##############