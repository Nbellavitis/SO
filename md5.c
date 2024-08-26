#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX_CHILD_QTY 10
#define INITIAL_FILES_PER_CHILD 2
#define MAX_MD5 32
#define MAX_PATH 128
#define MIN(x,y) (x<y) ? x:y 
char path[100];
int pipe_read(int fd, char *buff);
FILE * resultado;
int main(int argc, const char *argv[]){
    int child_qty=MIN(MAX_CHILD_QTY,argc-1);
    int parent_to_child_pipe[child_qty][2];
    int child_to_parent_pipe[child_qty][2];
    int  child_pid[child_qty];
    resultado= fopen("salida.txt", "wr");
    int files_assigned=1;
    for(int i=0;i<child_qty;i++){
        pipe(parent_to_child_pipe[i]);
        pipe(child_to_parent_pipe[i]);
        if ( (child_pid[i]=fork())== 0) {   // soy hijo
             close(parent_to_child_pipe[i][1]);
             close(child_to_parent_pipe[i][0]);
            
             dup2(parent_to_child_pipe[i][0], STDIN_FILENO);
             close(parent_to_child_pipe[i][0]);

              dup2(child_to_parent_pipe[i][1], STDOUT_FILENO);
              close(child_to_parent_pipe[i][1]);
          
            char *args[] = {"slave", NULL};
            execve(args[0], args, NULL);
            exit(EXIT_FAILURE);
        }
        close(parent_to_child_pipe[i][0]);
    }

 int total_files_to_process=argc-1;
for(int i=0;i<INITIAL_FILES_PER_CHILD;i++){
    for(int child_index=0;child_index<child_qty && files_assigned <= total_files_to_process;child_index++,files_assigned++){
        write(parent_to_child_pipe[child_index][1],argv[files_assigned],strlen(argv[files_assigned])+1);  
    }
}
//fd_set writefds;
//while(files_assigned< total_files_to_process){
//    for(int child_index=0;child_index<child_qty; child_index++){
//        FD_SET(parent_to_child_pipe[child_index][1], &writefds);
//    }
//    int n = select(); // escribir bien
//    for(int child_index=0; child_index < child_qty && files_assigned <= total_files_to_process; child_index++){
//        if(FD_ISSET(parent_to_child_pipe[child_index][1], &writefds)){
//            write(parent_to_child_pipe[child_index][1],argv[files_assigned],strlen(argv[files_assigned])+1);
//            files_assigned++;
//        }
//    }
//}



   fd_set readfds;
    int max_fd = -1;

for (int i = 0; i < argc-1; i++) { // hay que corregir esto
    FD_SET(child_to_parent_pipe[i%child_qty][0], &readfds);
            if (FD_ISSET(child_to_parent_pipe[i%child_qty][0], &readfds)) {
                int bytes_read = pipe_read(child_to_parent_pipe[i%child_qty][0], path);
                if (bytes_read < 0) {
                    perror("read");
                    exit(EXIT_FAILURE);
                } else if (bytes_read == 0) {
                    close(child_to_parent_pipe[i%child_qty][0]);
                } else {
                    fprintf(resultado, "ID:%d MD5:%s\n", child_pid[i%child_qty], path);
                    fflush(resultado);
                }
            }}
exit(EXIT_SUCCESS);


}
int pipe_read(int fd, char *buff){
    int i=0;
    char last_charater_read[1];
    last_charater_read[0]=1;

    while(last_charater_read[0]!=0 && last_charater_read[0]!='\n' && read(fd,last_charater_read,1)>0){
        buff[i++]=last_charater_read[0];
    }
    buff[i]=0;

    return i;
}

