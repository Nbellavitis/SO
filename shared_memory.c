#include "shared_memory.h"
#define SHARED_MEMORY_SIZE 1048576 // 1MB
sem_t *initialize_semaphore(const char *name, int value){
    sem_t * sem=sem_open(name,O_CREAT | O_RDWR,0666,value);
       if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    return sem;
}

char *create_shared_memory(const char * sh_mem_name, int *shm_fd){
   *shm_fd = shm_open(sh_mem_name, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if (*shm_fd == -1) {
        perror("shm_open");
        printf("No MD5 program active.\n");
        exit(EXIT_FAILURE);
    }
    char *shared_memory = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ, MAP_SHARED, *shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    return shared_memory;
}

void read_shared_memory(sem_t *shm_mutex_sem, sem_t *switch_sem, char *shared_memory) {
    int info_length = 0;

    while (1) {
        sem_wait(switch_sem);
        
        sem_wait(shm_mutex_sem);
        while (shared_memory[info_length] != '\n' && shared_memory[info_length] != '\t') {
            int i = strlen(shared_memory + info_length) + 1;
            if (i > 1) {
                printf("%s\n", shared_memory + info_length);
            }
            info_length += i;
        }

        if (shared_memory[info_length] == '\t') {
            sem_post(shm_mutex_sem);
            break;
        }
        sem_post(shm_mutex_sem);
    }
}