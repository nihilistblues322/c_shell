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
#else
#define LOG(...) ((void)0)
#endif

#endif
