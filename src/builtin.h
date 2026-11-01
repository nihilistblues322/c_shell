#ifndef BUILTIN_H
#define BUILTIN_H

typedef void (*builtin_fn)(int argc, char **argv);

struct item {
    const char *name;
    builtin_fn func;
};

const struct item *find_builtin(const char *name);

void run_echo(int argc, char **argv);

void run_type(int argc, char **argv);

void run_exit(int argc, char **argv);

void run_cd(int argc, char **argv);

void run_pwd(int argc, char **argv);

#endif
