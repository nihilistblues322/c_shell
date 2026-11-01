#ifndef BUILTIN_H
#define BUILTIN_H

/* Встроенные команды shell: выполняются внутри процесса shell (без fork). */

typedef void (*builtin_fn)(int argc, char **argv);

struct item {
    const char *name;
    builtin_fn func;
};

/* Возвращает запись таблицы builtin'ов или NULL, если name — не builtin. */
const struct item *find_builtin(const char *name);

void run_echo(int argc, char **argv);

void run_type(int argc, char **argv);

void run_exit(int argc, char **argv);

void run_cd(int argc, char **argv);

void run_pwd(int argc, char **argv);

#endif
