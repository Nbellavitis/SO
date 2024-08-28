#include "commons.h"
#include "pipe_master.h"


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

void close_pipes(int child_to_parent_pipe[][2], int parent_to_child_pipe[][2], int child_qty ){
    for (size_t i = 0; i < child_qty; i++)
    {
        close(child_to_parent_pipe[i][0]);
        close(parent_to_child_pipe[i][1]);
    }
}
void create_pipes(int parent_to_child_pipe[][2], int child_to_parent_pipe[][2], int index){
    if(pipe(parent_to_child_pipe[index]) == -1 || pipe(child_to_parent_pipe[index]) == -1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }
}