# Manual Técnico (Práctica 2 Sistemas Operativos 1 "A")

| Carpeta Raíz | Fecha |
| :---: | :---: | 
|[Carpeta](../../Practicas/Practica2/)|03/17|

## Introducción

<p style="text-align: justify;">
Dentro de la segunda práctica del laboratorio de Sistemas Operativos 2, la cual se realizo por medio de una distribución linux, se realizo un programa en el lenguaje de programación C el el que se crearon hilos para lectura masiva tanto de usuarios como de operaciones, utilizando (thread_t) y usando tanto mutex como un proceso de semáforo para que cada hilo leyera una linea de forma secuencial, evitando de esta forma que se sobreponga un proceso a otro en la lectura de los archivos (svc).<br/><br/>

Se empleo el lenguaje C como herramenta en la de los hilos y obtención de información de los mismos dentro de un Sistema Operativo de Linux (En este caso es Ubuntu o mejor dicho WUbuntu); que por medio de la aplicación padre se corrieron mediante la llamada al sistema <b>< Thread ></b> se crearon en este caso 3 hilos para la lectura del archivo de usuarios (clientes) y 4 hilos para la lectura de operaciones que se realizarán, de esta forma, tambien al leer el archivo, mediante estructuras (arreglos/arrays), se almacenó la información de los usuarios ingresados y se verificaba la "veracidad" o que la información de cada usuario:

1. No se repita.
2. No acepte valores no correspondientes tanto en el numero de cuenta como en el valor del monto o saldo del usuario.
3. Guarde la información del usuario una vez cumplan con los dos pasos anteriores.
<br/><br/>

Se pudo determinar que la implementación de hilos para la lectura de los archivos especificados no mostró ningun problema durante la ejecución del proyecto.
</p>

## Requisitos del Sistema

>- **_Sistema Operativo:_** Ubuntu 20.04 o superior
>- **_CPU:_** Intel Pentium D o AMD Athlon 64 (K8) 2.6GHz. (Requisitos Mínimo)
>- **_RAM:_** 600MB
>- **_Lenguajes Utilizados:_** C
>- **_Versión GCC Utilizado:_** GCC-11
>- **_Monitorizar Eventos_** SystemTap
>- **_IDE:_** Visual Studio Code

## Explicación del Código

### Definición de hilos

En el siguiente código que se muestra se define una estructura para que contenga una bandera de finalización del proceso (para que inicie otro hilo secuencialmente termine este), la variable de File, el cual hace la referencia del archivo que va a leer, el mutex y la condición que serviran para bloquear el acceso a los otros hilos para que solo uno haga un proceso y luego se encargue el que le sigue:

```
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
```

Tambien se cuenta con una clase void la cual se encarga de definir cuando se va a teerminar con el hilo (en este caso cuando termine su proceso respectivo "lectura de una linea").

### Definicion estructuras Usuario (Carga)

En sí es todo dentro de un "fork()" por lo que se va a ver un poco superficial (ya que se repite el procedimiento para el proceso subsiguiente).

```
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
```

Para esto se define la estructura **_Registro_**, esta se encarga de obtener la información de los usuarios y se usará posteriormente para la verificación de si hay un usuario repetido o no; **_registros_** es la variable que utilizará la estructura de **_Registro_** y es una varible o lista de 1000 espacios, la cual mediante la constante o variable **_num_registros_** se llenarán los espacios correspondientes de los nuevos usuarios que se registren.

También se tiene **_hilos_usuarios_** es un contador para los hilos que leen el archivo de los usuarios, por lo que incrementarán por cada lectura de linea del archivo (Tomar en cuenta que no leerá nunca la primer linea "Encabezado", ya que en ello se verá que el primer hilo a veces no concuerda con otros o es mayor a otras lineas).

Y por último se tiene "**_errores_usuarios_**" la cual cuenta con un espacio de 100 (se verá si se le extiende la cantidad de errores por si acaso), para la obtención de los errores de lectura del archivo. 

### Funciones Globales (Tanto para lectura como para verificación)

Se tendrán las siguientes líneas

```
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

bool Cuenta_Positiva(const char *cadena) {
    if (*cadena == '\0')
        return false;
    
    dígitos
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
    
    dígitos
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
```

Son funciones que tienen la función tanto de verificar si un usuario o un valor de cuenta/saldo/repetición de usuario hay, en el caso que retornen un "**false**" se verá que inmediatamente la tabla de errores obtendŕa ese error.

### Depositos

```
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
    token = strtok(NULL, ","); //cuenta 2
    int cuenta_2;
    if (Cuenta_2_Positiva(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Deposito]: Cuenta 2 no Valida");
        num_errores_operaciones++;
        return;
    }
    cuenta_2 = atoi(token);
    if (cuenta_2 != 0) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Deposito]: No debe haber una segunda cuenta en depositos");
        num_errores_operaciones++;
        return;
    }
    //monto
    token = strtok(NULL, ",");
    float monto;
    if (Saldo_Positivo(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Deposito]: Monto no Valido");
        num_errores_operaciones++;
        return;
    }
    monto = atof(token);
    if (existeCuenta(cuenta_1) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Deposito]: No existe la cuenta 1");
        num_errores_operaciones++;
        return;
    }
    for (int i = 0; i < num_registros; i++) {
        if (registros[i].no_cuenta == cuenta_1) {
            registros[i].saldo += monto;
            AumentarHiloOperaciones(numero_hilo);
            return;
        }
    }
}

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
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Deposito]: No existe la cuenta");
        num_errores_operaciones++;
        return;
    }

    for (int i = 0; i<num_registros; i++) {
        if (registros[i].no_cuenta == cuenta1) {
            registros[i].saldo += monto;
            return;
        }
    }
}
```

Se encargan de obtener la información recibida, verificar que esten bien los datos que leen y una vez confirmado todo, lo envían al arreglo de **registros**, generando al final (de la carga masiva) un archivo de reporte de la carga masiva.

### Retiros

```
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
    token = strtok(NULL, ","); //cuenta 2
    int cuenta_2;
    if (Cuenta_2_Positiva(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: Cuenta 2 no Valida");
        num_errores_operaciones++;
        return;
    }
    cuenta_2 = atoi(token);
    if (cuenta_2 != 0) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: No debe haber una segunda cuenta en depositos");
        num_errores_operaciones++;
        return;
    }
    //monto
    token = strtok(NULL, ",");
    float monto;
    if (Saldo_Positivo(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: Monto no Valido");
        num_errores_operaciones++;
        return;
    }
    monto = atof(token);
    if (existeCuenta(cuenta_1) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: No existe la cuenta 1");
        num_errores_operaciones++;
        return;
    }
    
    for (int i = 0; i < num_registros; i++) {
        if (registros[i].no_cuenta == cuenta_1) {
            if (registros[i].saldo < monto) {
                AumentarHiloOperaciones(numero_hilo);
                errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
                strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: Saldo insuficiente");
                num_errores_operaciones++;
                return;
            }
            registros[i].saldo -= monto;
            AumentarHiloOperaciones(numero_hilo);
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
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: No existe la cuenta");
        num_errores_operaciones++;
        return;
    }

    for (int i = 0; i<num_registros; i++) {
        if (registros[i].no_cuenta == cuenta1) {
            if (registros[i].saldo < monto) {
                printf("Error de monto\n");
                errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
                strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Retiro]: Saldo insuficiente");
                num_errores_operaciones++;
                return;
            }
            registros[i].saldo -= monto;
            return;
        }
    }
}
```

Se encarga de retirar de una cuenta el monto establecido, en caso que el monto sea 0 o menor a 0, o la cuenta no exista, mandará el error al reporte.

```
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
    token = strtok(NULL, ","); //cuenta 2
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
    float monto;
    if (Saldo_Positivo(token) == false) {
        AumentarHiloOperaciones(numero_hilo);
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: Monto no Valido");
        num_errores_operaciones++;
        return;
    }
    monto = atof(token);
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

    for (int i = 0; i < num_registros; i++) {
        for (int j = 0; j < num_registros; j++) {
            if ((registros[i].no_cuenta == cuenta_1) && (registros[j].no_cuenta == cuenta_2)) {
                if (registros[i].saldo < monto) {
                    AumentarHiloOperaciones(numero_hilo);
                    errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
                    strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: Saldo insuficiente");
                    num_errores_operaciones++;
                    return;
                }
                registros[i].saldo -= monto;
                registros[j].saldo += monto;
                AumentarHiloOperaciones(numero_hilo);
                return;
            }
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
        errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
        strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: No existe la cuenta");
        num_errores_operaciones++;
        return;
    }

    if (existeCuenta(cuenta2) == false) {
        printf("Cuenta 2 no existente\n");
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
                    errores_operaciones[num_errores_operaciones].linea  = num_linea_operacion;
                    strcpy(errores_operaciones[num_errores_operaciones].descripcion, "[Transferencia]: Saldo insuficiente");
                    num_errores_operaciones++;
                    return;
                }
                registros[j].saldo += monto;
                registros[i].saldo -= monto;
                return;
            }
        }
    }
}
```

A diferencia de los otros (Retiro/Deposito), requiere de una segunda cuenta, la cuenta a acreditar, la primer cuenta debita a la segunda cuenta... al igual que las otras, en el proceso de transferencia, deben existir estas dos cuentas, y el monto que va a debitar la cuenta a la otra no debe superar el saldo que tiene, de lo contrario... ERROR.

### Comandos Utilizados (Git hace mucho que no lo uso)

* git config --global user.name <userame>

```
git config --global user.name <username>
```

* git config --global user.email <email>

```
git config --global user.email <email>
```

* git config --global user.password <token_github>

```
git config --global user.password <token_github>
```


```
git init
```

```
git add .
```

```
git commit -m <nombre_commit>
```

```
git push origin main
```


