#ifndef _SHM
#define _SHM
#include "pipe_master.h"
#include <stdio.h>       // Standard I/O functions
#include <stdlib.h>      // Standard library functions (e.g., exit)
#include <string.h>      // String manipulation functions
#include <semaphore.h>   // Semaphore functions
#include <sys/mman.h>    // Memory management declarations (e.g., mmap, shm_open)
#include <sys/stat.h>    // File mode constants
#include <fcntl.h>       // File control options (e.g., O_CREAT, O_RDWR)
#include <unistd.h>      // POSIX operating system API (e.g., close, pipe)
#include <errno.h>       // Error handling (e.g., errno)
#define SHARED_MEMORY_SIZE (2 * 1048576) // 2MB
#define SHARED_MEMORY_NAME "shared_memory"
#define SHARED_MEMORY_NAME_LENGTH (strlen(SHARED_MEMORY_NAME) + 1)
#define SWITCH_SEM_NAME "switch_sem"

sem_t *initialize_semaphore(const char *name, int value,int * view_status);

void create_shared_memory(const char * sh_mem_name, int *shm_fd, char** shared_memory, int oflag, int prot);

void read_shared_memory( sem_t *switch_sem, char *shared_memory);


#endif