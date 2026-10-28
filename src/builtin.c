#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "builtin.h"
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/wait.h>
#include <stdbool.h>


static const char *names[] = {"echo", "exit", "type"};

int lookup(const char *s) {
    for (int i = 0; i < BUILTIN_COUNT; i++) {
        if (strcmp(names[i], s) == 0) {
            return i;
        }
    }
    return NOT_FOUND;
}

void run_echo(const char *args) {
    printf("%s\n", args ? args : "");
}

void run_type(const char *name) {
    if (!name) return;


    if (lookup(name) != UNKNOWN) {
        printf("%s is a shell builtin\n", name);
    } else {
        char path[1024];

        if (find_path(name, path, sizeof(path)) == OK) {
            printf("%s is %s\n", name, path);
        } else {
            printf("%s: not found\n", name);
        }
    }
}

int find_path(const char *name, char *out, size_t size) {
    bool found = false;

    const char *orig_path = getenv("PATH");
    if (orig_path == NULL) {
        return NOT_FOUND;
    }
    char *copy_path = strdup(orig_path);
    if (copy_path == NULL) {
        return NOT_FOUND;
    }
    char *dir = strtok(copy_path, ":");
    char candidate[1024];
    struct stat info;

    while (dir != NULL) {
        snprintf(candidate, sizeof(candidate), "%s/%s", dir, name);

        if (stat(candidate, &info) == 0 && S_ISREG(info.st_mode) && access(candidate, X_OK) == OK) {
            snprintf(out, size, "%s", candidate);
            found = true;
            break;
        }

        dir = strtok(NULL, ":");
    }


    free(copy_path);

    return found ? OK : NOT_FOUND;
}

int run_external(char *cmd, char *args) {
    if (cmd == NULL) {
        return NOT_FOUND;
    }

    char path[1024];
    int status;

    if (find_path(cmd, path, sizeof(path)) == 0) {
        pid_t pid = fork();

        if (pid == 0) {
            char *argv[64];
            build_argv(cmd, args, argv, 64);
            execv(path, argv);
            perror("execv error");
            exit(1);
        }
        if (pid > 0) {
            waitpid(pid, &status, 0);
            return OK;
        }
        if (pid < 0)
            return FORK_FAIL;
    }

    return NOT_FOUND;
}

void build_argv(char *cmd, char *args, char *argv[], int max) {
    int argc = 0;
    argv[argc++] = cmd;


    if (args != NULL) {
        char *tok = strtok(args, " ");

        while (tok != NULL && argc < max - 1) {
            argv[argc++] = tok;
            tok = strtok(NULL, " ");
        }
    }
    argv[argc] = NULL;
}
