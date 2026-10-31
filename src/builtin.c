#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "builtin.h"
#include "common.h"
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/wait.h>
#include "vec.h"
#include <stdbool.h>

#include "sbuf.h"


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

void run_echo(char *args) {
    printf("%s\n", args ? args : "");
}

void run_exit(char *args) {
    exit(0);
}

void run_type(char *args) {
    if (!args) return;

    char *sp = strchr(args, ' ');
    if (sp != NULL) *sp = '\0';

    const char *name = args;

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

int find_path(char *name, char *out, size_t size) {
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
    char candidate[PATH_MAX_LEN];
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

    char path[PATH_MAX_LEN];
    int status;

    if (find_path(cmd, path, sizeof(path)) == 0) {
        struct vec array;
        vec_init(&array);

        if (build_argv(cmd, args, &array) != OK) {
            vec_free(&array);
            perror("build_argv error");
            return ERR_NOMEM;
        }

        pid_t pid = fork();

        if (pid == 0) {
            LOG("exec %s argc-built\n", path);
            execv(path, array.items);
            perror("execv error");
            vec_free(&array);
            exit(1);
        }
        if (pid > 0) {
            waitpid(pid, &status, 0);
            vec_free(&array);
            return OK;
        }
        if (pid < 0) {
            vec_free(&array);
            return FORK_FAIL;
        }
    }

    return NOT_FOUND;
}

int build_argv(char *cmd, char *args, struct vec *array) {
    if (vec_push(array, cmd) != OK) {
        return ERR_NOMEM;
    }

    if (args != NULL) {
        for (char *tok = strtok(args, " "); tok != NULL; tok = strtok(NULL, " "))
            if (vec_push(array, tok) != OK) return ERR_NOMEM;
    }
    return vec_push(array, NULL);
}

void run_pwd(char *args) {
    (void) args;

    char buf[PATH_MAX_LEN];
    if (getcwd(buf, sizeof(buf)) != NULL) {
        printf("%s\n", buf);
    } else {
        perror("getcwd error");
    }
}

void run_cd(char *args) {
    if (args == NULL)
        return;

    const char *target = args;

    if (strcmp(args, "~") == OK) {
        target = getenv("HOME");
        if (target == NULL) {
            printf("cd: HOME not set\n");
            return;
        }
    }

    if (chdir(target) != OK) {
        printf("cd: %s: No such file or directory\n", args);
    }
}

int tokenize(const char *line, struct vec *out) {
    struct sbuf sb;
    sb_init(&sb);
    int in_quote = 0;
    int in_word = 0;

    for (const char *p = line; *p != '\0'; p++) {
        if (in_quote) {

        } else if (*p == '\'') {

        } else if (*p == ' ') {
            if (in_word) {
                
            }
        } else {
            sb_push(&sb, *p);
            in_word = 1;
        }
    }

    return OK;
}
