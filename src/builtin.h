#ifndef BUILTIN_H
#define BUILTIN_H

enum builtin {CMD_ECHO, CMD_EXIT, CMD_TYPE, BUILTIN_COUNT, UNKNOWN = -1};

int lookup(const char *s);
void run_echo(const char *args);
void run_type(const char *args);

#endif