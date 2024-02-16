#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define READ_END 0
#define WRITE_END 1

// Ctrl + C
void Syscall_SIGINT() {
    printf("Cerrando el Programa");
    exit(0);
}

int main() {
    pid_t pid_hijo1, pid_hijo2;
    int pipefd[2];

    signal(SIGINT, Syscall_SIGINT);

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_hijo1 = fork();
    if (pid_hijo1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid_hijo1 == 0) {
        close(pipefd[READ_END]);
        char msg1[] = "Hijo 1";
        write(pipefd[WRITE_END], msg1, sizeof(msg1));

        close(pipefd[WRITE_END]);
        
        execl("../child_process_1.bin", "child_process_1.bin", NULL);
        perror("Error al ejecutar el proceso hijo 1");
        exit(EXIT_FAILURE);
    }

    pid_hijo2 = fork();
    if (pid_hijo2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid_hijo2 == 0) {
        close(pipefd[READ_END]);
        char msg2[] = "Hijo 2";
        write(pipefd[WRITE_END], msg2, sizeof(msg2));

        close(pipefd[WRITE_END]);

        execl("../child_process_2.bin", "child_process_2.bin", NULL);
        perror("Error al ejecutar el proceso hijo 2");
        exit(EXIT_FAILURE);
    }

    close(pipefd[READ_END]);

    char buffer[100];
    read(pipefd[READ_END], buffer, sizeof(buffer));
    printf("Proceso: %s\n", buffer);

    close(pipefd[READ_END]);
    close(pipefd[WRITE_END]);

    char command[100];
    sprintf(command, "%s %d %d %s", "sudo stap ../trace.stp ", pid_hijo1, pid_hijo2, "  > syscalls.log");
    system(command);

    waitpid(pid_hijo1, NULL, 0);
    waitpid(pid_hijo2, NULL, 0);

    return 0;
}
