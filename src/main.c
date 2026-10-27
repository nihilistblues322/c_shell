#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtin.h"

int main(int argc, char *argv[]) {
    char buffer[1024];

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

        switch (lookup(cmd)) {
            case CMD_ECHO:
                run_echo(buffer);
                break;
            case CMD_EXIT:
                exit(0);
                break;
            case CMD_TYPE:
                run_type(buffer);
                break;
            default:
                printf("%s: command not found\n", cmd);
                break;
        }

        printf("$ ");
    }

    return 0;
}
