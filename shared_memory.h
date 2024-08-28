#ifndef _SHM
#define _SHM
#include "pipe_master.h"

sem_t *initialize_semaphore(const char *name, int value);

char *create_shared_memory(const char * sh_mem_name, int *shm_fd);

void read_shared_memory(sem_t *shm_mutex_sem, sem_t *switch_sem, char *shared_memory);
#endif