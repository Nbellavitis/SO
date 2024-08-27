
#ifndef _PIPEMASTER
#define _PIPEMASTER

#include <sys/wait.h>
#include <sys/select.h>

int pipe_read(int fd, char *buff);

#endif