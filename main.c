#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include "redir.h"

extern char **environ;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);
        return 1;
    }
    
    char *inp = argv[1];
    char *cmd = argv[2];
    char *out = argv[3];

    char **args;
    int arg_count;
    
    // Split cmd into executable and arguments
    split_command(cmd, &args, &arg_count);
    if (arg_count == 0) {
        fprintf(stderr, "No command specified.\n");
        return 1;
    }

    // Find absolute path if command is not an absolute path
    char *cmd_path = args[0];
    if (cmd_path[0] != '/') {
        char *absolute_path = find_absolute_path(args[0]);
        if (!absolute_path) {
            fprintf(stderr, "Command not found: %s\n", args[0]);
            return 1;
        }
        cmd_path = absolute_path;
    }

    // Fork a new process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) { // Child process
        // Open input file if specified
        if (strcmp(inp, "-") != 0) {
            int in_fd = open(inp, O_RDONLY);
            if (in_fd == -1) {
                perror("open input file");
                exit(1);
            }
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        
        // Open output file if specified
        int out_fd = STDOUT_FILENO;
        if (strcmp(out, "-") != 0) {
            out_fd = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (out_fd == -1) {
                perror("open output file");
                exit(1);
            }
            // Write the command to the output file
            dprintf(out_fd, "Command: %s\n", cmd);
        }
        dup2(out_fd, STDOUT_FILENO);
        if (out_fd != STDOUT_FILENO) close(out_fd);
        
        // Execute the command
        execve(cmd_path, args, environ);
        
        // If exec fails
        perror("execve");
        exit(1);
    } else { // Parent process
        // Wait for child process to complete
        int status;
        waitpid(pid, &status, 0);
    }

    // Free allocated memory for arguments and command path
    for (int i = 0; i < arg_count; i++) {
        free(args[i]);
    }
    free(args);

    if (cmd_path != args[0]) {
        free(cmd_path);
    }

    return 0;
}
