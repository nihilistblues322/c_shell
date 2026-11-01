#include <stdio.h>
#include <string.h>
#include "builtin.h"
#include "common.h"
#include "exec.h"
#include "tokenize.h"
#include "vec.h"


int main(int argc, char *argv[]) {
    char buffer[LINE_MAX_LEN];
    setbuf(stdout, NULL);
    printf("$ ");

    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        char *cmd = buffer;
        buffer[strcspn(cmd, "\n")] = '\0';
        LOG("\n========== new command ==========\n");
        LOG("main: read line <%s> (buffer=%p, on main's stack, reused by the next fgets)\n", buffer,
            (void *)buffer);

        struct vec vec;
        vec_init(&vec);
        int trc = tokenize(cmd, &vec);
        LOG("main: tokenize returned %d (%s)\n", trc,
            trc == OK ? "OK" : trc == ERR_SYNTAX ? "ERR_SYNTAX" : "error");
        if (trc != OK || vec.len == 0) {
            vec_free_all(&vec);
            printf("$ ");
            continue;
        }
        if (trc == ERR_SYNTAX) {
            perror("syntax error");
        }
        VEC_DUMP(&vec, "main: tokens (heap strings owned by main until vec_free_all)");

        const struct item *builtin = find_builtin(vec.items[0]);
        argc = vec.len;
        vec_push(&vec, NULL);
        LOG("main: argc=%d; pushed NULL terminator, vec.len is now %zu\n", argc, vec.len);
        LOG_ARGV("main: argv for the command", argc, vec.items);

        LOG("main: '%s' -> %s\n", vec.items[0],
            builtin ? "builtin (runs inside the shell process, no fork)"
            : "external program (fork + execv)");

        if (builtin != NULL) builtin->func(argc, vec.items);
        else {
            int run = run_external(vec.items);
            LOG("main: run_external returned %d (%s)\n", run,
                run == OK ? "OK" : run == NOT_FOUND ? "NOT_FOUND" : "FORK_FAIL");
            if (run == NOT_FOUND) {
                printf("%s: not found\n", vec.items[0]);
            } else if (run == FORK_FAIL) {
                perror("fork error");
            }
        }
        LOG("main: cleanup, free every token string and the vec array\n");
        vec_free_all(&vec);

        printf("$ ");
    }

    return 0;
}
