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
    printf("%s\n", args ? args : "");
}

void run_type(const char *name) {
    if (!name) return;
    if (lookup(name) != UNKNOWN) {
        printf("%s is a shell builtin\n", name);
    } else {
        printf("%s: not found\n", name);
    }
}
