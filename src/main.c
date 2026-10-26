#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  char buffer[1024];

  setbuf(stdout, NULL);

  printf("$ ");

  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {

    buffer[strcspn(buffer, "\n")] = '\0';

    printf("%s: command not found\n", buffer);

    if (strcmp(buffer, "exit") == 0) {
      break;
    }

    printf("$ ");
  }

  return 0;
}
