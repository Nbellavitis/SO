#include "commons.h"
#include "pipe_master.h"
#define MAX_CHILD_QTY 5
#define INITIAL_FILES_PER_CHILD 2
#define MAX_MD5 32
#define MAX_PATH 128
#define MIN(x,y) (x<y) ? x:y 
char path[100];
void set_fd(int child_to_parent_pipe[][2],int flag, int * max_fd, fd_set * readfds,int child_qty);
FILE * resultado;


void set_fd(int child_to_parent_pipe[][2],int flag, int * max_fd, fd_set * readfds,int child_qty){
     for (int i = 0; i < child_qty; i++) {
        if (child_to_parent_pipe[i][flag] > *max_fd) {
            *max_fd = child_to_parent_pipe[i][flag];
        }
        FD_SET(child_to_parent_pipe[i][flag], readfds);
    }
}

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



   fd_set readfds,writefds;
    int max_read = -1;
    int max_write=-1;

for (int i = 0; i < argc-1; ) { // hay que corregir esto --creo que ahi esta? (feo igual )
    set_fd(child_to_parent_pipe,0,&max_read,&readfds,child_qty);
    set_fd(parent_to_child_pipe,1,&max_write,&writefds,child_qty);
         select(max_read+1,&readfds,NULL,NULL,NULL);
         select(max_write+1,NULL,&writefds,NULL,NULL);
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
                    if (files_assigned < argc && FD_ISSET(parent_to_child_pipe[i%child_qty][1], &writefds)) {
                    write(parent_to_child_pipe[i%child_qty][1], argv[files_assigned], strlen(argv[files_assigned]) + 1); //VER SI ESTA LIBERADO PARA ESCRIBIR
                    files_assigned++;
                    }
                    i++;
                }
            }}
exit(EXIT_SUCCESS);
}


