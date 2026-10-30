#ifndef BUILTIN_H
#define BUILTIN_H
#include "vec.h"
#include <stddef.h>

typedef void (*builtin_fn)(char *args);

struct item {
    const char *name;
    builtin_fn func;
};

const struct item *find_builtin(const char *name);

void run_echo(char *args);

void run_type(char *args);

void run_exit(char *args);

void run_cd(char *args);

int find_path(char *name, char *out, size_t size);

int run_external(char *cmd, char *args);

int build_argv(char *cmd, char *args, struct vec *array);

void run_pwd(char *args);

#endif
