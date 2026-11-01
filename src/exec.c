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
        LOG("find_path: PATH is not set -> NOT_FOUND\n");
        return NOT_FOUND;
    }
    char *copy_path = strdup(orig_path);
    if (copy_path == NULL) {
        LOG("find_path: strdup failed (out of memory) -> NOT_FOUND\n");
        return NOT_FOUND;
    }
    LOG("find_path: searching '%s' in PATH; copy_path=%p (heap copy, strtok will modify it)\n", name,
        (void *)copy_path);
    char *dir = strtok(copy_path, ":");
    char candidate[PATH_MAX_LEN];
    struct stat info;

    while (dir != NULL) {
        snprintf(candidate, sizeof(candidate), "%s/%s", dir, name);

        if (stat(candidate, &info) == 0 && S_ISREG(info.st_mode) && access(candidate, X_OK) == OK) {
            snprintf(out, size, "%s", candidate);
            found = true;
            LOG("find_path:   %s -> FOUND (regular file + executable), out=\"%s\"\n", candidate, out);
            break;
        }
        LOG("find_path:   %s -> no\n", candidate);

        dir = strtok(NULL, ":");
    }

    free(copy_path);
    LOG("find_path: result for '%s': %s\n", name, found ? "OK" : "NOT_FOUND");

    return found ? OK : NOT_FOUND;
}

int run_external(char **argv) {
    if (argv == NULL || argv[0] == NULL) return NOT_FOUND;
    char *cmd = argv[0];

    char path[PATH_MAX_LEN];

    LOG("run_external: command '%s' (argv is borrowed from main's vec, NULL-terminated)\n", cmd);
    if (find_path(cmd, path, sizeof(path)) != OK) {
        LOG("run_external: '%s' not found in PATH, nothing to run\n", cmd);
        return NOT_FOUND;
    }

    LOG("run_external: full path \"%s\", calling fork()\n", path);
    pid_t pid = fork();
    int status;

    if (pid == 0) {
        LOG("child  pid=%d: I am a copy of the shell; execv(\"%s\") replaces me with that program\n",
            (int)getpid(), path);
        execv(path, argv);
        LOG("child  pid=%d: execv returned, so it FAILED\n", (int)getpid());
        perror("execv error");
        exit(1);
    }
    if (pid > 0) {
        LOG("parent pid=%d: fork created child pid=%d, waiting in waitpid\n", (int)getpid(),
            (int)pid);
        waitpid(pid, &status, 0);
        LOG("parent pid=%d: child %d finished, %s=%d\n", (int)getpid(), (int)pid,
            WIFEXITED(status) ? "exit code" : "killed by signal",
            WIFEXITED(status) ? WEXITSTATUS(status) : WTERMSIG(status));
        return OK;
    }
    if (pid < 0) {
        LOG("run_external: fork failed\n");
        return FORK_FAIL;
    }


    return NOT_FOUND;
}
