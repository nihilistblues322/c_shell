#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  char buffer[1024];

  setbuf(stdout, NULL);

  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    printf("$ ");

    buffer[strcspn(buffer, "\n")] = "\0";

    printf("%s: command not found\n", buffer);
  }

  return 0;
}
