#include "commons.h"
#include "pipe_master.h"
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include "shared_memory.h"
#define MAX_CHILD_QTY 5
#define INITIAL_FILES_PER_CHILD 2
#define MAX_MD5 32
#define MAX_PATH 128
#define MIN(x,y) (x<y) ? x:y
#define MAX(x,y) (x>y) ? x:y 
char path[100];
void set_fd(int child_to_parent_pipe[][2],int flag, int * max_fd, fd_set * readfds,int child_qty);
void start_shared_memory(int *shm_fd, char **shared_memory, sem_t **shm_sem,sem_t **switch_sem, int *vision_opened);
FILE * resultado;

int is_fd_open(int fd) {
    if (fcntl(fd, F_GETFD) == -1) {
        if (errno == EBADF) {
            return 0;  // File descriptor is not open
        }else{
            perror("closed");
            exit(EXIT_FAILURE);
        }
    }
    return 1;  // File descriptor is open
}




void set_fd(int child_to_parent_pipe[][2],int flag, int * max_fd, fd_set * readfds,int child_qty){
     for (int i = 0; i < child_qty; i++) {
        if (child_to_parent_pipe[i][flag] > *max_fd) {
            *max_fd = child_to_parent_pipe[i][flag];
        }
        FD_SET(child_to_parent_pipe[i][flag], readfds);
    }
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


void send_to_process(int child_index, int quantity, int * files_assigned,int total_files_to_process, int parent_to_child_pipe[][2], const char *argv[]){
    for(int i=0;i<quantity && *files_assigned <= total_files_to_process;i++) {
        write(parent_to_child_pipe[child_index][1], argv[*files_assigned], strlen(argv[*files_assigned]) + 1); // revisar argv
        (*files_assigned)++;
    }
}
void send_initial_files(int child_qty, int total_files_to_process, int * files_assigned , int parent_to_child_pipe[][2], const char *argv[]){
    for(int child_index=0; child_index<child_qty && *files_assigned <= total_files_to_process;child_index++){
        send_to_process(child_index, MIN(total_files_to_process,INITIAL_FILES_PER_CHILD), files_assigned,total_files_to_process,parent_to_child_pipe, argv);
    }
}

 void start_shared_memory(int *shm_fd, char **shared_memory, sem_t **shm_sem,sem_t **switch_sem, int *vision_opened){
    *shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if (*shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    *shared_memory = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, *shm_fd, 0);
    if (*shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    
    sleep(2);

    *shm_sem = sem_open(SHM_SEM_NAME, 1);
    if (*shm_sem != SEM_FAILED) {
        (*vision_opened)++;
    }

    *switch_sem = sem_open(SWITCH_SEM_NAME, 0);
    if (*switch_sem != SEM_FAILED) {
        (*vision_opened)++;
    }
 

 if (*vision_opened == 0) {
        printf("No view detected - No semaphore initialization\n");
    }

   if (ftruncate(*shm_fd, SHARED_MEMORY_SIZE) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
}

 }


int main(int argc, const char *argv[]){
    int child_qty=MIN(MAX_CHILD_QTY,argc-1);
    int parent_to_child_pipe[child_qty][2];
    int child_to_parent_pipe[child_qty][2];
    int  child_pid[child_qty];
    resultado= fopen("salida.txt", "wr");
    int files_assigned=1;
    if (!isatty(STDOUT_FILENO)) {
        write(STDOUT_FILENO, SHARED_MEMORY_NAME,strlen(SHARED_MEMORY_NAME)+1);
       
    } else {
        printf("%s\n", SHARED_MEMORY_NAME);
    }
    int view_status = 0;
    int shm_fd;
    char *shared_memory;
    sem_t *shm_sem;
    sem_t *switch_sem;
    start_shared_memory(&shm_fd, &shared_memory, &shm_sem,&switch_sem , &view_status);

    for(int i=0;i<child_qty;i++){ // podria llamar a este loop como una nueva funcion

        create_pipes(parent_to_child_pipe,child_to_parent_pipe,i);

        create_child_process(parent_to_child_pipe,child_to_parent_pipe,i, child_pid);

        handle_pipes_parent(parent_to_child_pipe,child_to_parent_pipe,i);
    }

 int total_files_to_process=argc-1;
    send_initial_files(child_qty,total_files_to_process,&files_assigned, parent_to_child_pipe, argv);

    fd_set readfds,writefds;
   
    int file_index=0;
    int info_length = strlen("ID:%d MD5:%s\n") + MAX_MD5 + MAX_PATH ;

while (file_index < total_files_to_process) {
    int max_read=-1;
    int max_write=-1;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    set_fd(child_to_parent_pipe, 0, &max_read, &readfds, child_qty);
    set_fd(parent_to_child_pipe, 1, &max_write, &writefds, child_qty);

    if (select(max_read + 1, &readfds, NULL, NULL, NULL) == -1) {
        perror("select read");
        exit(EXIT_FAILURE);
    }

    if (select(max_write + 1, NULL, &writefds, NULL, NULL) == -1) {
        perror("select write");
        exit(EXIT_FAILURE);
    }
   
     if (view_status == 2) {
            sem_wait(shm_sem);
        }
    for (int i = 0; i < child_qty; i++) {
        
   
        if (FD_ISSET(child_to_parent_pipe[i][0], &readfds)) {
            int bytes_read = pipe_read(child_to_parent_pipe[i][0], path);
            if (bytes_read < 0) {
                perror("read");
                exit(EXIT_FAILURE);
            } else {
                
                fprintf(resultado, "ID:%d MD5:%s\n", child_pid[i], path);
                fflush(resultado);
                snprintf(shared_memory + file_index * info_length, info_length, "ID:%d MD5:%s\n", child_pid[i], path);
                if (view_status == 2) {
                sem_post(switch_sem);
                sem_post(shm_sem); 
                }
                if (files_assigned < argc && FD_ISSET(parent_to_child_pipe[i][1], &writefds)) {
                    send_to_process(i, 1, &files_assigned, total_files_to_process, parent_to_child_pipe, argv);
                }
                file_index++;
               
            }
        }
    }
   
}
    if (view_status == 2) {
    sem_wait(shm_sem);
     sprintf(shared_memory + file_index * info_length, "\t");
    sem_post(shm_sem);
    sem_post(switch_sem);
    }
   
close_pipes(child_to_parent_pipe, parent_to_child_pipe, child_qty);
sem_close(shm_sem);
sem_unlink(SHM_SEM_NAME);
sem_close(switch_sem);
sem_unlink(SWITCH_SEM_NAME);
close(shm_fd);
shm_unlink(SHARED_MEMORY_NAME);
fclose(resultado);
exit(EXIT_SUCCESS);
}


