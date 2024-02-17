# Manual Técnico (Práctica 1 Sistemas Operativos 1 "A")

| Carpeta Raíz | Fecha |
| :---: | :---: | 
|[Carpeta](../../Practicas/Practica1/)|02/19|

## Introducción

<p style="text-align: justify;">
Dentro de la primera práctica del laboratorio de Sistemas Operativos 1, la cual se realizo por medio de una distribución linux, se realizo un programa en el lenguaje de programación C el el que se crearon dos procesos hijos y mediante el proceso padre, se utilizo fork para correrlos; monitoreando las llamadas "Read" y "Write" que realizaban los procesos hijos mediante un tercer proceso hijo que se hizo el cual utilizaba un archivo de SystemTap (.stp), enviando de esta forma a un archivo .log la información obtenida en el SystemTap. También se uso la captura de la señal SIGINT (Ctrl + C) para la finalización del proceso padre y con ello la obtención de los datos de las lecturas, leídas y movidas de puntero que se realizaron por los procesos hijos.<br/><br/>

Se empleo el lenguaje C como herramenta en la creación de los procesos hijos y obtención de información de los mismos dentro de un Sistema Operativo de Linux (En este caso es Ubuntu o mejor dicho WUbuntu); que por medio de la aplicación padre se corrieron mediante la llamada al sistema <b>< fork() ></b> se crearon dos procesos hijos que corrieron las aplicaciones .bin de lo que serían los que escribirían en un archivo de texto, por otra parte, se creo un tercer proceso hijo para el monitor que estaría obteniendo la información mediante SystemTap todas las llamadas de "Read" y "Write" que hicieron los procesos hijos que corrieron las aplicaciones, enviando cada llamada a un archivo "syscalls.log" el cual obtendría el nombre del proceso, pid del proceso, el tipo de llamada que realizo y la fecha y hora en la que se hizo; se uso el uso de la funcion o método "waitpid()" el cual espera a que los proceoso hijos finalicen, las funciones "fopen()", "fwrite()" y "fclose()" para la escritura del archivo <b>practica1.txt</b>.<br/><br/>

Se pudo determinar que la implementación de los procesos hijos y la escritura/reescritura de los archivos especificados no mostró ningun problema durante la ejecución del proyecto.
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

### Procesos Hijos (child_process_1//child_process_2)

Mediante la siguiente línea de código, se creará el archivo "practica1.txt" el cual se va a llenar al correr el programa, tener en cuenta que el archivo se vaciará si se vuelve a correr el programa:

```
#define FILEPATH "../File/practica1.txt"
...
file = open(FILEPATH, O_RDWR | O_CREAT | O_TRUNC, 0644);
if (file < 0) {
    perror("[Child] Error al abrir archivo");
    exit(EXIT_FAILURE);
}
...
```

Se cuenta con los siguientes comandos a comprender
* **_O_RDWR:_** Nos indica que se abrirá el archivo para lectura y escritura.
* **_O_CREAT:_** En caso que no exista el archivo lo va a crear.
* **_O_TRUNC:_** Si el archivo ya existe, lo va a vaciar.
> Permiso 0664
* **_0:_** Indica que es archivo regular.
* **_6:_** Indica que el propietario del archivo tiene permisos de lectura y escritura.
* **_4:_** Indica que el grupo del archivo tiene permisos de lectura y escritura.
* **_4:_** Indica que los demas usuarios tienen permisos de lectura y escritura.


<br/><br/>

Para el siguiente fragmento de código, se tiene una variable volatil la cual va a hacer referencia a cuando este corriendo el hijo mediante el padre. Se utilizará un "rand()" para dar un número aleatorio en este caso se usará el "%3% para que quede de valores entre 0 y 2 para los siguientes casos.

```
int total_calls = 0;
int read_calls = 0;
int write_calls = 0;
int seek_calls = 0;
volatile sig_atomic_t sigint_received = 0;
struct timespec ts;
while(!sigint_received) {
    call = rand() % 3;
    switch (call) {
    case 0: //read file
        int readed = read(file, palabra, 8);
        //if (readed > 0) {
        //    palabra[readed] = '\0';
        //    printf("%s\n", palabra);
        //} else {
        //    printf("Error en la lectura\n");
        //}
        total_calls++;
        read_calls++;
        break;
    case 1: //write file
        for (int i = 0; i < 8; i++) { palabra[i] = rand() % 26 + 'a'; }
        palabra[8] = '\0';
        write(file, palabra, 8);
        total_calls++;
        write_calls++;
        break;
    case 2: //seek file
        lseek(file, 0, SEEK_SET);
        total_calls++;
        seek_calls++;
        break;
    }
    ts.tv_sec = rand() % 3 + 1;
    ts.tv_nsec = 0;
    nanosleep(&ts, NULL);
}
```

Los siguientes casos son:
* **_0:_** Se leerán 8 caracteres del archivo. Al finalizar sumará la variable de Reads y la del total de llamadas. 
* **_1:_** Escribirá en el archivo "practica1.txt" una cadena de 8 caracteres alfanuméricos aleatorios. 
* **_2:_** Moverá el puntero a la primera posición.

Para la función del tiempo de descanso entre llamadas está "ts" el cual por medio de "nanosleep" por cada ciclo dentro de while usará un número aleatorio de tiempo (en segundos) de 1 a 3 para la siguiente llamada.

### Proceso Padre

En sí es todo dentro de un "fork()" por lo que se va a ver un poco superficial (ya que se repite el procedimiento para el proceso subsiguiente).

```
signal(SIGINT, Syscall_SIGINT);

pid_t pid_hijo1 = fork();
if (pid_hijo1 == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
}

if (pid_hijo1 == 0) {
    execl("../child_process_1.bin", "child_process_1.bin", NULL);
    perror("Error al ejecutar el proceso hijo 1");
    exit(EXIT_FAILURE);
} else {
    pid_t pid_hijo2 = fork();
    if (pid_hijo2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid_hijo2 == 0) {
        execl("../child_process_2.bin", "child_process_2.bin", NULL);
        perror("Error al ejecutar el proceso hijo 2");
        exit(EXIT_FAILURE);
    } else {
        pid_t pid_monitor = fork();
        if (pid_monitor == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
        }
        if (pid_monitor == 0) {
        // execl("../monitor_process.bin", "monitor_process.bin", NULL);
        char command[80];
        sprintf(command, "%s %d %d %s", "sudo stap ../trace.stp ", pid_hijo1, pid_hijo2, "  > syscalls.log");
        system(command);
        //perror("Error al ejecutar el proceso monitor");
        exit(EXIT_FAILURE);
        } else {
        int status1, status2, status3;
        waitpid(pid_hijo1, &status1, 0);
        waitpid(pid_hijo2, &status2, 0);
        waitpid(pid_monitor, &status3, 0);
        }
    }
}
```

Se tiene el primer "fork()" el cual correrá al primer hijo (en este caso será **_child_process_1.bin_**). Una vez corra el primer hijo, correrá el segundo hijo (**_child_process_2.bin_**) y lo mismo sería para el tercer hijo/proceso que se crea, el cual es el monitor, este se encargará de enviar a un archivo (.log) la información obtenida mediante el SystemTap (**_trace.stp_**) toda la información de lectura (**_read_**) y escritura (**_write_**) que realizaron los procesos hijos 1 y 2.

Finalmente, con el comando o la señal SIGINT (Ctrl + C) se van a cerrar los procesos hijos y con ello la escritura de los archivos mencionados anteriormente (**_practica1.txt_** && **_syscalls.log_**). Con todo esto, posteriormente imprime por cada hijo el total de llamadas que hiso, las llamadas de lectura, escritura y movida de puntero.


### Monitor de Eventos SystemTap (trace.stp)

Se tendrán las siguientes líneas

```
probe syscall.read {
    if (pid() == $1 || pid() == $2) {
        ts = gettimeofday_s();
        printf("%s %d: %s (%s)\n", execname(), pid(), name, ctime(ts))
    }
}

probe syscall.write {
    if (pid() == $1 || pid() == $2) {
        ts = gettimeofday_s();
        printf("%s %d: %s (%s)\n", execname(), pid(), name, ctime(ts))
    }
}
```

En la cual la primera función `syscall.read` se encarga de obtener las llamadas de lectura para los PID's que ingresan como parámetros, de esta forma filtra la información recibida; la segunda función `syscall.write` se encarga de obtener las llamadas de escritura para los PID's de la misma forma que la de lectura.

### Comandos Utilizados para conversión de Archivos .c a .bin

* gcc -o <nombre_archivo_salida>.bin <nombre_archivo>.c

```
gcc -o child_process_1.bin child_process_1.c
```

### Comando para correr SystemTap

> Instalación Stap

```
sudo apt install systemtap linux-headers-$(uname -r)
```

> Ejecutar stap

```
sudo stap trace.stp 
```

### Comando para cambiar compilador GCC predeterminado

```
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100 --slave /usr/bin/g++ g++ /usr/bin/g++-11 --slave /usr/bin/gcov gcov /usr/bin/gcov-11

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 90 --slave /usr/bin/g++ g++ /usr/bin/g++-12 --slave /usr/bin/gcov gcov /usr/bin/gcov-12

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 80 --slave /usr/bin/g++ g++ /usr/bin/g++-10 --slave /usr/bin/gcov gcov /usr/bin/gcov-10
```