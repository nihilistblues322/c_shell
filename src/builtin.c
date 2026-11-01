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
            LOG("find_builtin: '%s' -> builtin #%zu\n", name, i);
            return &items[i];
        }
    }
    LOG("find_builtin: '%s' -> not a builtin\n", name);
    return NULL;
}

void run_echo(int argc, char **argv) {
    LOG_ARGV("run_echo", argc, argv);
    LOG("run_echo: printing %d word(s), argv[0] (\"%s\") is the command name and is skipped\n",
        argc > 0 ? argc - 1 : 0, argc > 0 ? argv[0] : "");
    for (int i = 1; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
}

void run_exit(int argc, char **argv) {
    (void) argc;
    (void) argv;
    LOG("run_exit: calling exit(0), shell terminates\n");
    exit(0);
}

void run_type(int argc, char **argv) {
    LOG_ARGV("run_type", argc, argv);
    if (argc < 2) {
        LOG("run_type: no argument after 'type', nothing to look up\n");
        return;
    }

    char *sp = strchr(argv[1], ' ');
    if (sp != NULL) *sp = '\0';

    const char *name = argv[1];

    LOG("run_type: looking up '%s' (1st: builtin table, 2nd: PATH)\n", name);
    if (find_builtin(name) != NULL) {
        LOG("run_type: '%s' found in the builtin table\n", name);
        printf("%s is a shell builtin\n", name);
    } else {
        char path[PATH_MAX_LEN];

        if (find_path(name, path, sizeof(path)) == OK) {
            LOG("run_type: '%s' found in PATH -> \"%s\"\n", name, path);
            printf("%s is %s\n", name, path);
        } else {
            LOG("run_type: '%s' is neither a builtin nor in PATH\n", name);
            printf("%s: not found\n", name);
        }
    }
}

void run_pwd(int argc, char **argv) {
    (void) argc;
    (void) argv;
    LOG_ARGV("run_pwd", argc, argv);

    char buf[PATH_MAX_LEN];
    if (getcwd(buf, sizeof(buf)) != NULL) {
        LOG("run_pwd: getcwd -> \"%s\" (written into stack buffer buf)\n", buf);
        printf("%s\n", buf);
    } else {
        LOG("run_pwd: getcwd failed\n");
        perror("getcwd error");
    }
}

void run_cd(int argc, char **argv) {
    LOG_ARGV("run_cd", argc, argv);
    if (argc < 2) {
        LOG("run_cd: no argument, nothing to do\n");
        return;
    }

    const char *target = argv[1];

    if (strcmp(argv[1], "~") == OK) {
        target = getenv("HOME");
        LOG("run_cd: '~' expands to HOME=%s (borrowed from the environment, do not free)\n",
            target ? target : "(unset)");
        if (target == NULL) {
            printf("cd: HOME not set\n");
            return;
        }
    }

    LOG("run_cd: chdir(\"%s\")\n", target);
    if (chdir(target) != OK) {
        LOG("run_cd: chdir failed\n");
        printf("cd: %s: No such file or directory\n", argv[1]);
    } else {
        LOG("run_cd: ok, the shell process changed its own working directory\n");
    }
}
