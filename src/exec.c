#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"
#include "exec.h"
#include "vec.h"

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
    int tried = 0;

    while (dir != NULL) {
        tried++;
        snprintf(candidate, sizeof(candidate), "%s/%s", dir, name);

        if (stat(candidate, &info) == 0 && S_ISREG(info.st_mode) && access(candidate, X_OK) == OK) {
            snprintf(out, size, "%s", candidate);
            found = true;
            break;
        }

        dir = strtok(NULL, ":");
    }

    free(copy_path);
    LOG("[path] name=\"%s\" tried=%d -> %s%s%s\n", name, tried, found ? "\"" : "",
        found ? out : "not found", found ? "\"" : "");

    return found ? OK : NOT_FOUND;
}

int run_external(char **argv) {
    if (argv == NULL || argv[0] == NULL) return NOT_FOUND;
    char *cmd = argv[0];

    char path[PATH_MAX_LEN];

    if (find_path(cmd, path, sizeof(path)) != OK) {
        return NOT_FOUND;
    }

    int argc = 0;
    while (argv[argc] != NULL) argc++;
    LOG("[exec] path=\"%s\"\n", path);
    LOG_ARGV("argv", argc, argv);

    pid_t pid = fork();
    int status;

    if (pid == 0) {
        LOG("[child] pid=%d execv\n", (int)getpid());
        execv(path, argv);
        perror("execv error");
        exit(1);
    }
    if (pid > 0) {
        LOG("[fork] parent=%d child=%d\n", (int)getpid(), (int)pid);
        waitpid(pid, &status, 0);
        LOG("[wait] pid=%d %s=%d\n", (int)pid, WIFEXITED(status) ? "exit" : "signal",
            WIFEXITED(status) ? WEXITSTATUS(status) : WTERMSIG(status));
        return OK;
    }
    if (pid < 0) {
        return FORK_FAIL;
    }


    return NOT_FOUND;
}
