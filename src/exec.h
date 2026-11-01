#ifndef EXEC_H
#define EXEC_H

#include <stddef.h>

#include "vec.h"

int find_path(char *name, char *out, size_t size);

int run_external(char **argv);

#endif
