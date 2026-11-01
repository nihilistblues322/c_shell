#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

#define LINE_MAX_LEN 1024
#define PATH_MAX_LEN 1024
#define ARGV_MAX     64

#define OK         0
#define NOT_FOUND -1
#define FORK_FAIL -2
#define ERR_NOMEM -3
#define ERR_SYNTAX -4

#ifdef DEBUG
#define LOG(...) fprintf(stderr, "[dbg] " __VA_ARGS__)

/* Debug: print argc and every argv[i] (argv may or may not end with NULL). */
static inline void log_argv(const char *who, int argc, char **argv) {
    fprintf(stderr, "[dbg] %s: argc=%d argv:", who, argc);
    for (int i = 0; i < argc; i++) fprintf(stderr, " [%d]=\"%s\"", i, argv[i]);
    fprintf(stderr, "\n");
}
#define LOG_ARGV(who, argc, argv) log_argv((who), (argc), (argv))
#else
#define LOG(...) ((void)0)
#define LOG_ARGV(who, argc, argv) ((void)0)
#endif

#endif
