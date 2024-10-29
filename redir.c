#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "redir.h"

void split_command(const char *cmd, char ***args, int *arg_count) {
    *arg_count = 0;
    char *cmd_copy = strdup(cmd);
    char *token = strtok(cmd_copy, " ");
    
    // Allocate array for arguments
    *args = malloc(sizeof(char *) * 10); // assuming max 10 arguments for simplicity
    
    while (token != NULL && *arg_count < 10) {
        (*args)[*arg_count] = strdup(token);
        (*arg_count)++;
        token = strtok(NULL, " ");
    }
    (*args)[*arg_count] = NULL; // NULL-terminate the array
    
    free(cmd_copy);
}

char *find_absolute_path(char *command) {
    char *path_env = getenv("PATH");
    if (!path_env) return NULL;
    
    char *path_copy = strdup(path_env);
    char *dir = strtok(path_copy, ":");
    
    while (dir != NULL) {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);
        
        struct stat buffer;
        if (stat(full_path, &buffer) == 0 && (buffer.st_mode & S_IXUSR)) {
            free(path_copy);
            return strdup(full_path);
        }
        
        dir = strtok(NULL, ":");
    }
    
    free(path_copy);
    return NULL;
}
