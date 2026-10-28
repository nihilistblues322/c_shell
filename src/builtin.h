#ifndef BUILTIN_H
#define BUILTIN_H

#define OK         0
#define NOT_FOUND -1
#define FORK_FAIL -2

enum builtin { CMD_ECHO, CMD_EXIT, CMD_TYPE, BUILTIN_COUNT, UNKNOWN = -1 };

int lookup(const char *s);

void run_echo(const char *args);

void run_type(const char *args);

int find_path(const char *name, char *path, size_t size);

int run_external(char *cmd, char *args);

void build_argv(char *cmd, char *args, char *argv[], int max);

#endif
