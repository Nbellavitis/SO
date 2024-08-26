#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define CHILD_QTY 10
#define INITIAL_FILES_PER_CHILD 2
int parent_to_child_pipe[CHILD_QTY][2];
int child_to_parent_pipe[CHILD_QTY][2];
int  child_pid[CHILD_QTY];
int pipe_read(int fd, char *buff);
FILE * resultado;
int main(int argc, const char *argv[]){
 int files_assigned=1;
    for(int i=0;i<CHILD_QTY;i++){
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
for(int i=0;i<total_files_to_process;i++){
    for(int child_index=0;child_index<CHILD_QTY && files_assigned <= total_files_to_process;child_index++,files_assigned++){
        write(parent_to_child_pipe[child_index][1],argv[files_assigned],strlen(argv[files_assigned])+1);  
    }
}

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

