#ifndef REDIR_H
#define REDIR_H

void split_command(const char *cmd, char ***args, int *arg_count);
char *find_absolute_path(char *command);

#endif
