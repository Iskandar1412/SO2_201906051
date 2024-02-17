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

### Procesos Hijo (child_process_1//child_process_2)

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

