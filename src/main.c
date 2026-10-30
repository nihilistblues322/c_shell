#include <stdio.h>
#include <string.h>
#include "builtin.h"
#include "common.h"


int main(int argc, char *argv[]) {
    char buffer[LINE_MAX_LEN];
    setbuf(stdout, NULL);
    printf("$ ");

    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        char *cmd = buffer;
        char *args = NULL;
        buffer[strcspn(cmd, "\n")] = '\0';

        char *space = strchr(buffer, ' ');

        if (space != NULL) {
            *space = '\0';
            args = space + 1;
        }

        const struct item *builtin = find_builtin(cmd);

        if (builtin != NULL) builtin->func(args);
        else {
            int run = run_external(cmd, args);
            if (run == NOT_FOUND) {
                printf("%s: not found\n", cmd);
            } else if (run == FORK_FAIL) {
                perror("fork error");
            }
        }

        printf("$ ");
    }

    return 0;
}
