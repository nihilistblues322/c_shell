#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  char buffer[1024];

  setbuf(stdout, NULL);

  printf("$ ");

  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {

    buffer[strcspn(buffer, "\n")] = '\0';

    if (strcmp(buffer, "exit") == 0) {
      break;
    } else if (strncmp(buffer, "echo ", 5) == 0) {
      printf("%s\n", buffer + 5);
    } else {
      printf("%s: command not found\n", buffer);
    }

    printf("$ ");
  }

  return 0;
}
