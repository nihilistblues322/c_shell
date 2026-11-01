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
        LOG("\n[line] \"%s\"\n", buffer);

        struct vec vec;
        vec_init(&vec);
        int trc = tokenize(cmd, &vec);
        if (trc != OK) LOG("[tokenize] rc=%d\n", trc);
        if (trc != OK || vec.len == 0) {
            vec_free_all(&vec);
            printf("$ ");
            continue;
        }
        if (trc == ERR_SYNTAX) {
            perror("syntax error");
        }
        VEC_DUMP(&vec, "tokens");

        const struct item *builtin = find_builtin(vec.items[0]);
        argc = vec.len;
        vec_push(&vec, NULL);
        LOG_ARGV("argv", argc, vec.items);
        LOG("[run] \"%s\" %s\n", vec.items[0], builtin ? "builtin" : "external");

        if (builtin != NULL) builtin->func(argc, vec.items);
        else {
            int run = run_external(vec.items);
            LOG("[rc] run_external=%d\n", run);
            if (run == NOT_FOUND) {
                printf("%s: not found\n", vec.items[0]);
            } else if (run == FORK_FAIL) {
                perror("fork error");
            }
        }
        vec_free_all(&vec);

        printf("$ ");
    }

    return 0;
}
