#ifndef _COMMONS
#define _COMMONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_MD5 32
#define MAX_PATH 128
#define OFFSET 3
#define MAX_OUTPUT_LENGTH (MAX_PATH + MAX_MD5 + OFFSET)
#define MIN(x, y) ((x<y) ? (x):(y))
#define MAX(x, y) ((x>y) ? (x):(y))
#define HANDLE_ERROR(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
#endif