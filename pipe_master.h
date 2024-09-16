#ifndef PIPE_MASTER_H  // Header guard to prevent multiple inclusions
#define PIPE_MASTER_H

#include <unistd.h>    // For close(), dup2(), read()
#include <sys/wait.h>  // For wait()
#include <sys/select.h> // For select()

// Function to read from a pipe until a newline or null character is encountered
int pipe_read(int fd, char *buff, int length);

// Function to handle pipe setup for child process
void handle_pipes_child(int parent_to_child_pipe[][2], int child_to_parent_pipe[][2], int index);

// Function to handle pipe setup for parent process
void handle_pipes_parent(int parent_to_child_pipe[][2], int child_to_parent_pipe[][2], int index);

// Function to close all pipes in the parent process

void create_pipes(int parent_to_child_pipe[][2], int child_to_parent_pipe[][2], int index);

void close_brothers_pipes(int child_to_parent_pipe[][2], int parent_to_child_pipe[][2],int index);

#endif // PIPE_MASTER_H
