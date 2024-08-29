#include <sys/types.h>
#include <sys/wait.h>
#include "commons.h"
#include "shared_memory.h"
int main (int argc, const char * argv[]) {
    sem_unlink(SWITCH_SEM_NAME);
    sem_unlink(SHM_SEM_NAME);
    sem_t *sem =initialize_semaphore(SHM_SEM_NAME,1);
    sem_t *sem_switch=initialize_semaphore(SWITCH_SEM_NAME,0);
    int shm_fd;
    char *shared_memory;
    char shm_name[MAX_PATH] = {0};
    if (argc > 1){
        shared_memory=create_shared_memory(argv[1],&shm_fd);
    }
    else {
        pipe_read(STDIN_FILENO, shm_name);
        if (shm_name[0] == '\0'){
            printf("PATH_LIMITATION_ERROR");
            exit(1);
        }

        shared_memory = create_shared_memory(shm_name, &shm_fd);
    }
    read_shared_memory(sem, sem_switch, shared_memory);
    close(shm_fd);
    sem_close(sem);
    sem_close(sem_switch);
}
