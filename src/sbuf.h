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

/* Debug: print sbuf contents to stderr. Compiles to nothing in Release. */
#ifdef DEBUG
void sb_debug_dump(const struct sbuf *sb, const char *label);
#define SBUF_DUMP(sb, label) sb_debug_dump((sb), (label))
#else
#define SBUF_DUMP(sb, label) ((void)0)
#endif

#endif
