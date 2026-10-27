#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "builtin.h"
#include <stdlib.h>
#include <unistd.h>

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
        char *orig_path = getenv("PATH");
        if (orig_path == NULL) {
            printf("%s: not found\n", name);
            return;
        }
        char *copy_path = strdup(orig_path);
        if (copy_path == NULL) {
            printf("%s: not found\n", name);
            return;
        }
        char *dir = strtok(copy_path, ":");
        char candidate[1024];
        struct stat info;

        while (dir != NULL) {
            snprintf(candidate, sizeof(candidate), "%s/%s", dir, name);

            if (stat(candidate, &info) == 0 &&
                S_ISREG(info.st_mode) &&
                access(candidate, X_OK) == 0) {
                printf("%s is %s\n", name, candidate);
                break;
            }

            dir = strtok(NULL, ":");
        }

        if (dir == NULL) {
            printf("%s: not found\n", name);
        }

        free(copy_path);
    }
}
