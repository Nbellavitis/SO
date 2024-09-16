// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "commons.h"
#include "pipe_master.h"


    int pipe_read(int fd, char *buff, int length){
        int bytes_read = read(fd, buff, length);
        if (bytes_read == 0) {
            // EOF reached, pipe closed
            return 0;
        } else if (bytes_read > 0) {
            buff[--bytes_read] = '\0';
        }
        return bytes_read;
    }


void handle_pipes_child(int parent_to_child_pipe[][2], int child_to_parent_pipe[][2], int index) {
    close(parent_to_child_pipe[index][1]);
    close(child_to_parent_pipe[index][0]);

    dup2(parent_to_child_pipe[index][0], STDIN_FILENO);
    close(parent_to_child_pipe[index][0]);

    dup2(child_to_parent_pipe[index][1], STDOUT_FILENO);
    close(child_to_parent_pipe[index][1]);
}
void handle_pipes_parent(int parent_to_child_pipe[][2], int child_to_parent_pipe[][2], int index) {
    close(parent_to_child_pipe[index][0]);
    close(child_to_parent_pipe[index][1]);
}

void close_pipes(int child_to_parent_pipe[][2], int parent_to_child_pipe[][2], int child_qty, int child_pids[]) {
    for (size_t i = 0; i < child_qty; i++)
    {
        close(child_to_parent_pipe[i][0]);
        close(parent_to_child_pipe[i][1]);
        int status;
        int pid = waitpid(child_pids[i], &status, 0);
        if(pid == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    }
}

void create_pipes(int parent_to_child_pipe[][2], int child_to_parent_pipe[][2], int index) {
    if(pipe(parent_to_child_pipe[index]) == -1 || pipe(child_to_parent_pipe[index]) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
}

void close_brothers_pipes(int child_to_parent_pipe[][2], int parent_to_child_pipe[][2],int index) {
    for(int i=0;i<index;i++) {
        for(int j=0;j<2;j++) {
            close( child_to_parent_pipe[i][j]);
            close(parent_to_child_pipe[i][j]);
        }
    }
}
