#include "commons.h"
#include "pipe_master.h"
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#define MAX_CHILD_QTY 5
#define INITIAL_FILES_PER_CHILD 2
#define MAX_MD5 32
#define MAX_PATH 128
#define MIN(x,y) (x<y) ? x:y 
char path[100];

void set_fd(int child_to_parent_pipe[][2],int flag, int * max_fd, fd_set * readfds,int child_qty);
FILE * resultado;

int is_fd_open(int fd) {
    if (fcntl(fd, F_GETFD) == -1) {
        if (errno == EBADF) {
            return 0;  // File descriptor is not open
        }
    }
    return 1;  // File descriptor is open
}

void close_pipes(int child_to_parent_pipe[][2], int parent_to_child_pipe[][2], int child_qty ){
    for (size_t i = 0; i < child_qty; i++)
    {
        close(child_to_parent_pipe[i][0]);
        close(parent_to_child_pipe[i][1]);
    }
}




void set_fd(int child_to_parent_pipe[][2],int flag, int * max_fd, fd_set * readfds,int child_qty){
     for (int i = 0; i < child_qty; i++) {
        if (child_to_parent_pipe[i][flag] > *max_fd) {
            *max_fd = child_to_parent_pipe[i][flag];
        }
        FD_SET(child_to_parent_pipe[i][flag], readfds);
    }
}
void handle_pipes_child(int parent_to_child_pipe[][2], int child_to_parent_pipe[][2], int index){
    close(parent_to_child_pipe[index][1]);
    close(child_to_parent_pipe[index][0]);

    dup2(parent_to_child_pipe[index][0], STDIN_FILENO);
    close(parent_to_child_pipe[index][0]);

    dup2(child_to_parent_pipe[index][1], STDOUT_FILENO);
    close(child_to_parent_pipe[index][1]);
}
void handle_pipes_parent(int parent_to_child_pipe[][2], int child_to_parent_pipe[][2], int index){
    close(parent_to_child_pipe[index][0]);
    close(child_to_parent_pipe[index][1]);
}

void create_child_process(int parent_to_child_pipe[][2], int child_to_parent_pipe[][2], int index, int * child_pid){
    if((child_pid[index]=fork())== -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }else if( child_pid[index]== 0) {

        handle_pipes_child(parent_to_child_pipe, child_to_parent_pipe, index);

        char *args[] = {"slave", NULL};
        execve(args[0], args, NULL);
        exit(EXIT_FAILURE);
    }
}
void create_pipes(int parent_to_child_pipe[][2], int child_to_parent_pipe[][2], int index){
    if(pipe(parent_to_child_pipe[index]) == -1 || pipe(child_to_parent_pipe[index]) == -1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }
}

void send_to_process(int child_index, int quantity, int * files_assigned, int parent_to_child_pipe[][2], const char *argv[]){
    for(int i=0;i<quantity;i++) {
        write(parent_to_child_pipe[child_index][1], argv[*files_assigned], strlen(argv[*files_assigned]) + 1); // revisar argv
        (*files_assigned)++;
    }
}
void send_initial_files(int child_qty, int total_files_to_process, int * files_assigned , int parent_to_child_pipe[][2], const char *argv[]){
    for(int child_index=0; child_index<child_qty && *files_assigned <= total_files_to_process;child_index++){
        send_to_process(child_index, INITIAL_FILES_PER_CHILD, files_assigned,parent_to_child_pipe, argv);
    }
}
void close_pipes_that_are_not_mine(int parent_to_child_pipe[][2], int child_to_parent_pipe[][2], int my_index){
    for(int i=0; i<my_index; i++){
        if(i!=my_index){
            for(int j=0; j<2; j++){
                close(parent_to_child_pipe[i][j]);
                close(child_to_parent_pipe[i][j]);
            }
        }
    }
}

int main(int argc, const char *argv[]){
    int child_qty=MIN(MAX_CHILD_QTY,argc-1);
    int parent_to_child_pipe[child_qty][2];
    int child_to_parent_pipe[child_qty][2];
    int  child_pid[child_qty];
    resultado= fopen("salida.txt", "wr");
    int files_assigned=1;
    for(int i=0;i<child_qty;i++){ // podria llamar a este loop como una nueva funcion
        close_pipes_that_are_not_mine(parent_to_child_pipe,child_to_parent_pipe,i);

        create_pipes(parent_to_child_pipe,child_to_parent_pipe,i);

        create_child_process(parent_to_child_pipe,child_to_parent_pipe,i, child_pid);

        handle_pipes_parent(parent_to_child_pipe,child_to_parent_pipe,i);
    }

 int total_files_to_process=argc-1;
    send_initial_files(child_qty,total_files_to_process,&files_assigned, parent_to_child_pipe, argv);

    fd_set readfds,writefds;
    int max_read = -1;
    int max_write=-1;
    int file_index=0;
    while(file_index < total_files_to_process){ // hay que corregir esto --creo que ahi esta? (feo igual )
        set_fd(child_to_parent_pipe,0,&max_read,&readfds,child_qty);
        set_fd(parent_to_child_pipe,1,&max_write,&writefds,child_qty);
        //select(MAX(max_read,max_write)+1,&readfds,&writefds,NULL,NULL); (ESTO NO FUNCIONA NO SE PORQUE AAAA)
        select(max_read+1,&readfds,NULL,NULL,NULL);
        select(max_write+1,NULL,&writefds,NULL,NULL);
        for(int i=0;i<child_qty;i++){
            if (FD_ISSET(child_to_parent_pipe[i][0], &readfds)) {
                int bytes_read = pipe_read(child_to_parent_pipe[i][0], path);
                if (bytes_read < 0) {
                    perror("read");
                    exit(EXIT_FAILURE);
                } else if (bytes_read == 0) {
                    close(child_to_parent_pipe[i][0]);
                } else {
                    fprintf(resultado, "ID:%d MD5:%s\n", child_pid[i], path);
                    fflush(resultado);
                    if (files_assigned < argc && FD_ISSET(parent_to_child_pipe[i][1], &writefds)) {
                        send_to_process(i, 1, &files_assigned, parent_to_child_pipe, argv);
                    }
                    file_index++;
                }
            }
        }
    }
close_pipes(child_to_parent_pipe, parent_to_child_pipe, child_qty);

//    for (size_t i = 0; i < child_qty; i++)
//    {
//        if(is_fd_open(child_to_parent_pipe[i][0])){
//            fprintf(resultado,"Child to parent, read %d\n",i);
//            fflush(resultado);
//            close(child_to_parent_pipe[i][0]);
//        }
//        if(is_fd_open(child_to_parent_pipe[i][1])){
//            fprintf(resultado,"Child to parent, write %d\n",i);
//            fflush(resultado);
//            close(child_to_parent_pipe[i][1]);
//        }
//        if(is_fd_open(parent_to_child_pipe[i][0])){
//            fprintf(resultado,"parent to child, read %d\n",i);
//            fflush(resultado);
//            close(parent_to_child_pipe[i][0]);
//        }
//        if(is_fd_open(parent_to_child_pipe[i][1])){
//            fprintf(resultado,"parent to child, write %d\n",i);
//            fflush(resultado);
//            close(parent_to_child_pipe[i][1]);
//
//        }
//    }

exit(EXIT_SUCCESS);
}


