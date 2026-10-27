#include <stdio.h>
#include <string.h>
#include "builtin.h"

static const char *names[] = {"echo", "exit", "type"};

int lookup(const char *s) {
    for (int i = 0; i < BUILTIN_COUNT; i++) {
        if (strcmp(names[i], s) == 0) {
            return i;
        }
    }
    return UNKNOWN;
}


void run_echo(const char *args) {
    if (strncmp(args, "echo ", 5) == 0) {
        printf("%s\n", args + 5);
    }
}

void run_type(const char *args) {
    const char *name = args + 5;

    for (int i = 0; i < BUILTIN_COUNT; i++) {
        if (strcmp(name, names[i]) == 0) {
            printf("%s is a shell builtin\n", names[i]);
        }
    }
}
