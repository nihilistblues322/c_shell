#ifndef SBUF_H
#define SBUF_H

#include <stddef.h>

struct sbuf {
    char *data;
    size_t len;
    size_t cap;
};

void sb_init(struct sbuf *sb);

int sb_push(struct sbuf *sb, char c);

char *sb_finish(struct sbuf *sb);

void sb_free(struct sbuf *sb);

#endif
