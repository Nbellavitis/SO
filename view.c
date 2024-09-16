// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <sys/wait.h>
#include "commons.h"
#include "shared_memory.h"

int main(int argc, const char *argv[]) {
    sem_unlink(SWITCH_SEM_NAME);
    sem_t *sem_switch = initialize_semaphore(SWITCH_SEM_NAME, 0, NULL);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    int shm_fd;
    char *shared_memory;
    char shm_name[MAX_PATH] = {0};
    if (argc > 1) {
        create_shared_memory(argv[1], &shm_fd, &shared_memory, O_RDWR, PROT_READ);
    } else {
        pipe_read(STDIN_FILENO, shm_name, SHARED_MEMORY_NAME_LENGTH);
        if (shm_name[0] == '\0') {
            HANDLE_ERROR("PATH_LIMITATION_ERROR");
        }

        create_shared_memory(shm_name, &shm_fd, &shared_memory, O_RDWR, PROT_READ);
    }
    read_shared_memory(sem_switch, shared_memory);
    close(shm_fd);
    sem_close(sem_switch);
}
