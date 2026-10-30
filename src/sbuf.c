#include "sbuf.h"
#include <stddef.h>
#include <stdlib.h>

#include "common.h"


void sb_init(struct sbuf *sb) {
    sb->data = NULL;
    sb->len = 0;
    sb->cap = 0;
}

int sb_push(struct sbuf *sb, char c) {
    if (sb->len + 1 >= sb->cap) {
        const size_t new_cap = sb->cap == 0 ? 4 : sb->cap * 2;
        char *tmp = realloc(sb->data, new_cap * sizeof(char));
        if (tmp == NULL) return ERR_NOMEM;
        sb->data = tmp;
        sb->cap = new_cap;
    }
    sb->data[sb->len++] = c;
    return OK;
}

char *sb_finish(struct sbuf *sb) {
    if (sb->data == NULL) {
        char *e = malloc(1);
        if (e == NULL) return NULL;
        e[0] = '\0';
        return e;
    }
    char *tmp = sb->data;
    sb->data[sb->len] = '\0';

    sb->data = NULL;
    sb->len = 0;
    sb->cap = 0;

    return tmp;
}

void sb_free(struct sbuf *sb) {
    free(sb->data);
    sb->data = NULL;
    sb->cap = 0;
    sb->len = 0;
}
