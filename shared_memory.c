#include "shared_memory.h"

sem_t *initialize_semaphore(const char *name, int value) {
    sem_t *sem = sem_open(name, O_CREAT, 0666, value);
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
    int i = 0;

    while (1) {
        sem_wait(switch_sem);
        
        sem_wait(shm_mutex_sem);
        while (shared_memory[i] != '\n' && shared_memory[i] != '\t') {
            printf("%c",shared_memory[i++]);
        }

        if (shared_memory[i] == '\t') {
            sem_post(shm_mutex_sem);
            break;
            
        }
        printf("%c",shared_memory[i++]);
        sem_post(shm_mutex_sem);
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
 

 if (*vision_opened <=1 ) {
        printf("No view detected - No semaphore initialization\n");
    }

   if (ftruncate(*shm_fd, SHARED_MEMORY_SIZE) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
}
}