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
   printf("Cerrando el Programa\n");
   exit(0);
}

int main() {

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

   // pid_t pid_hijo2 = fork();
   // if (pid_hijo2 == -1) {
   //    perror("fork");
   //    exit(EXIT_FAILURE);
   // }

   // if (pid_hijo2 == 0) {
   //    close(pipefd[READ_END]);
   //    char msg2[] = "Hijo 2";
   //    write(pipefd[WRITE_END], msg2, sizeof(msg2));

   //    close(pipefd[WRITE_END]);

   //    execl("../child_process_2.bin", "child_process_2.bin", NULL);
   //    perror("Error al ejecutar el proceso hijo 2");
   //    exit(EXIT_FAILURE);
   // }
   
   // close(pipefd[READ_END]);
   // close(pipefd[WRITE_END]);

   // char buffer[80];
   // read(pipefd[READ_END], buffer, sizeof(buffer));
   // printf("Proceso: %s\n", buffer);

   // close(pipefd[READ_END]);

   // char command[80];
   // sprintf(command, "%s %d %d %s", "sudo stap ../trace.stp ", pid_hijo1, pid_hijo2, "  > syscalls.log");
   // system(command);

   // waitpid(pid_hijo1, NULL, 0);
   // waitpid(pid_hijo2, NULL, 0);

   return 0;
}
