// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "shared_memory.h"
#include "commons.h"

sem_t *initialize_semaphore(const char *name, int value, int *view_status) {
    sem_t *sem = sem_open(name, O_CREAT | O_EXCL, 0666, value);

    if (sem == SEM_FAILED) {
        if (errno == EEXIST && view_status != NULL) {
            sem = sem_open(name, 0);
            if (sem != SEM_FAILED) {
                (*view_status)++;
            }
        } else {
            HANDLE_ERROR("sem_open (create semaphore)");
        }
    }

    return sem;
}

void create_shared_memory(const char *sh_mem_name, int *shm_fd, char **shared_memory, int oflag, int prot) {
    *shm_fd = shm_open(sh_mem_name, oflag, S_IRWXU | S_IRWXG | S_IRWXO);
    if (*shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    *shared_memory = mmap(NULL, SHARED_MEMORY_SIZE, prot, MAP_SHARED, *shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
}

void read_shared_memory(sem_t *switch_sem, char *shared_memory) {
    int i = 0;

    while (1) {
        sem_wait(switch_sem);
        while (shared_memory[i] != '\n' && shared_memory[i] != '\t') {
            printf("%c", shared_memory[i++]);
        }

        if (shared_memory[i] == '\t') {
            break;
        }
        printf("%c", shared_memory[i++]);
    }
}

