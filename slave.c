// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "pipe_master.h"
#include "commons.h"

int main(int argc, const char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    char path[MAX_PATH];
    char *md5_cmd = "md5sum %s";
    char command[MAX_PATH + strlen(md5_cmd)];

    while (pipe_read(STDIN_FILENO, path, MAX_PATH + 1) > 0) {
        if (path[0] == 0) {
            // No more files to read, exit the loop
            break;
        }
        char md5[MAX_OUTPUT_LENGTH];
        sprintf(command, md5_cmd, path);
        FILE *fp = popen(command, "r");
        if (fp == NULL) {
            HANDLE_ERROR("popen");
        }
        fgets(md5, MAX_OUTPUT_LENGTH, fp);
        pclose(fp);
        write(STDOUT_FILENO, md5, strlen(md5));
    }
    close(STDOUT_FILENO);
    exit(EXIT_SUCCESS);
}
