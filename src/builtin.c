#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "common.h"
#include "exec.h"

static const struct item items[] = {
    {"echo", run_echo},
    {"exit", run_exit},
    {"type", run_type},
    {"pwd", run_pwd},
    {"cd", run_cd},
};

const struct item *find_builtin(const char *name) {
    size_t n = sizeof(items) / sizeof(items[0]);

    for (size_t i = 0; i < n; i++) {
        if (strcmp(name, items[i].name) == 0) {
            return &items[i];
        }
    }
    return NULL;
}

void run_echo(int argc, char **argv) {
    LOG_ARGV("echo", argc, argv);
    for (int i = 1; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
}

void run_exit(int argc, char **argv) {
    (void) argc;
    (void) argv;
    LOG("[exit] code=0\n");
    exit(0);
}

void run_type(int argc, char **argv) {
    if (argc < 2) {
        return;
    }

    char *sp = strchr(argv[1], ' ');
    if (sp != NULL) *sp = '\0';

    const char *name = argv[1];
    LOG("[type] name=\"%s\"\n", name);

    if (find_builtin(name) != NULL) {
        printf("%s is a shell builtin\n", name);
    } else {
        char path[PATH_MAX_LEN];

        if (find_path(name, path, sizeof(path)) == OK) {
            printf("%s is %s\n", name, path);
        } else {
            printf("%s: not found\n", name);
        }
    }
}

void run_pwd(int argc, char **argv) {
    (void) argc;
    (void) argv;

    char buf[PATH_MAX_LEN];
    if (getcwd(buf, sizeof(buf)) != NULL) {
        LOG("[pwd] cwd=\"%s\"\n", buf);
        printf("%s\n", buf);
    } else {
        perror("getcwd error");
    }
}

void run_cd(int argc, char **argv) {
    if (argc < 2) {
        return;
    }

    const char *target = argv[1];

    if (strcmp(argv[1], "~") == OK) {
        target = getenv("HOME");
        if (target == NULL) {
            printf("cd: HOME not set\n");
            return;
        }
    }

    LOG("[cd] arg=\"%s\" target=\"%s\"\n", argv[1], target);
    if (chdir(target) != OK) {
        printf("cd: %s: No such file or directory\n", argv[1]);
    } else {
    }
}
