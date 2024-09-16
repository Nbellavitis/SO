// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "commons.h"
#include "pipe_master.h"
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include "shared_memory.h"
#define MAX_CHILD_QTY 5
#define INITIAL_FILES_PROPORTION 10
#define OUTPUT_FORMAT "PID:%d MD5:%s\n"
#define INFO_LENGTH ((int)strlen(OUTPUT_FORMAT) + MAX_MD5 + MAX_PATH)

FILE * result;

typedef struct {
    int parent_to_child_pipe[MAX_CHILD_QTY][2];
    int child_to_parent_pipe[MAX_CHILD_QTY][2];
    int shm_fd;
    char *shared_memory;
    sem_t *switch_sem;
} ipc_resources;

typedef struct {
    int child_qty;
    int files_assigned;
    int total_files_to_process;
    int child_pid[MAX_CHILD_QTY];
    int view_status;
} program_params;

void set_fd(ipc_resources *ipc, int flag, int *max_fd, fd_set *read_fds, program_params *params);
void create_child_process(ipc_resources *ipc, program_params *params, int index);
void send_to_process(int child_index, int quantity, int *files_assigned, int total_files_to_process, ipc_resources *ipc, const char *argv[]);
void send_initial_files(program_params *params, ipc_resources *ipc, const char *argv[], int files_per_child);
void start_shared_memory(ipc_resources *ipc);

int is_fd_open(int fd) {
    if (fcntl(fd, F_GETFD) == -1) {
        if (errno == EBADF) {
            return 0;  
        } else {
            perror("closed");
            exit(EXIT_FAILURE);
        }
    }
    return 1;  
}

void set_fd(ipc_resources *ipc, int flag, int *max_fd, fd_set *read_fds, program_params *params) {
    for (int i = 0; i < params->child_qty; i++) {
        if (ipc->child_to_parent_pipe[i][flag] > *max_fd) {
            *max_fd = ipc->child_to_parent_pipe[i][flag];
        }
        FD_SET(ipc->child_to_parent_pipe[i][flag], read_fds);
    }
}

void create_child_process(ipc_resources *ipc, program_params *params, int index) {
    if((params->child_pid[index] = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if(params->child_pid[index] == 0) {
        handle_pipes_child(ipc->parent_to_child_pipe, ipc->child_to_parent_pipe, index);
        close_brothers_pipes(ipc->parent_to_child_pipe, ipc->child_to_parent_pipe, index);
        char *args[] = {"slave", NULL};
        execve(args[0], args, NULL);
        exit(EXIT_FAILURE);
    }
}

void send_to_process(int child_index, int quantity, int *files_assigned, int total_files_to_process, ipc_resources *ipc, const char *argv[]) {
    for(int i = 0; i < quantity && *files_assigned <= total_files_to_process; i++) {
        write(ipc->parent_to_child_pipe[child_index][1], argv[*files_assigned], MAX_PATH + 1);
        (*files_assigned)++;
    }
}

void send_initial_files(program_params *params, ipc_resources *ipc, const char *argv[], int files_per_child) {
    for(int child_index = 0; child_index < params->child_qty && params->files_assigned <= params->total_files_to_process; child_index++) {
        send_to_process(child_index, MIN(params->total_files_to_process, files_per_child), &params->files_assigned, params->total_files_to_process, ipc, argv);
    }
}

void start_shared_memory(ipc_resources *ipc) {

    create_shared_memory(SHARED_MEMORY_NAME,&ipc->shm_fd,&ipc->shared_memory, O_CREAT | O_RDWR, PROT_READ | PROT_WRITE);

    sleep(2);
    if (!isatty(STDOUT_FILENO)) {
        write(STDOUT_FILENO, SHARED_MEMORY_NAME, strlen(SHARED_MEMORY_NAME) + 1);
    } else {
        printf("%s\n", SHARED_MEMORY_NAME);
    }

    if (ftruncate(ipc->shm_fd, SHARED_MEMORY_SIZE) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
}
void initialize_slaves(program_params *params, ipc_resources *ipc) {
    for(int i = 0; i < params->child_qty; i++) {
        create_pipes(ipc->parent_to_child_pipe, ipc->child_to_parent_pipe, i);
        create_child_process(ipc, params, i);
        handle_pipes_parent(ipc->parent_to_child_pipe, ipc->child_to_parent_pipe, i);
    }
}
void initialize_params(program_params *params, int argc ) {
    params->child_qty = MIN(MAX_CHILD_QTY, argc - 1);
    params->files_assigned = 1;
    params->total_files_to_process = argc - 1;
    params->view_status=0;
}
void connect_shared_memory(ipc_resources *ipc, int *view_status) {
    start_shared_memory(ipc);
    ipc->switch_sem = initialize_semaphore(SWITCH_SEM_NAME, 0,view_status );
    if (*view_status <= 0){
        printf("No view detected - No semaphore initialization\n");
    }
}
void print_output(int *printed_files,program_params *params, ipc_resources *ipc, int index, int argc,const char *argv[], char * path ) {
    fprintf(result, OUTPUT_FORMAT, params->child_pid[index], path);
    fflush(result);
    sprintf(ipc->shared_memory + (*printed_files) * INFO_LENGTH, OUTPUT_FORMAT, params->child_pid[index], path);
    if (params->view_status == 1) {
        sem_post(ipc->switch_sem);
    }
    if (params->files_assigned < argc) {
        send_to_process(index, 1, &(params->files_assigned), params->total_files_to_process, ipc, argv);
    }
    (*printed_files)++;
}
void read_from_slaves(program_params *params, ipc_resources *ipc, int argc, const char * argv[], fd_set * read_fds, int * printed_files) {
    char path[MAX_OUTPUT_LENGTH];
    for (int i = 0; i < params->child_qty; i++) {
        if (FD_ISSET(ipc->child_to_parent_pipe[i][0], read_fds)) {
            int bytes_read = pipe_read(ipc->child_to_parent_pipe[i][0], path, MAX_OUTPUT_LENGTH);
            if (bytes_read < 0) {
                perror("read");
                exit(EXIT_FAILURE);
            } else {
                print_output(printed_files, params, ipc, i, argc,argv, path);
            }
        }
    }
}
void process_remaining_files(program_params *params,ipc_resources *ipc, int argc, const char * argv[], int * printed_files ){
    fd_set read_fds;
    while (*printed_files < params->total_files_to_process) {
        int max_read = -1;
        FD_ZERO(&read_fds);
        set_fd(ipc, 0, &max_read, &read_fds, params);

        if (select(max_read + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select read");
            exit(EXIT_FAILURE);
        }
        read_from_slaves(params,ipc, argc, argv, &read_fds, printed_files); //also sends more files to process
    }
}
void close_resources(program_params * params, ipc_resources *ipc, int printed_files ){
    if (params->view_status == 1) {
        sprintf(ipc->shared_memory + printed_files * INFO_LENGTH, "\t");
        sem_post(ipc->switch_sem);
    }

    close_pipes(ipc->child_to_parent_pipe, ipc->parent_to_child_pipe, params->child_qty);
    sem_unlink(SWITCH_SEM_NAME);
    sem_close(ipc->switch_sem);
    close(ipc->shm_fd);
    shm_unlink(SHARED_MEMORY_NAME);
    fclose(result);
    exit(EXIT_SUCCESS);
}

int main(int argc, const char *argv[]) {
    if (argc == 1) {
        printf("Usage: %s <file1> <file2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    setvbuf(stdout, NULL, _IONBF, 0);

    program_params params;
    initialize_params(&params, argc);

    ipc_resources ipc;
    result = fopen("salida.txt", "w");
    connect_shared_memory(&ipc, &(params.view_status));

    initialize_slaves(&params, &ipc);

    int files_per_child = MAX(1, (argc - 1) / (params.child_qty * INITIAL_FILES_PROPORTION));
    send_initial_files(&params, &ipc, argv, files_per_child);

    int printed_files = 0;
    process_remaining_files(&params,&ipc,argc,argv,&printed_files);

    close_resources(&params, &ipc, printed_files);
}
