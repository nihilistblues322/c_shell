#include "vec.h"
#include <stdlib.h>

#include "common.h"


void vec_init(struct vec *v) {
    v->len = 0;
    v->cap = 0;
    v->items = NULL;
}

void vec_free(struct vec *v) {
    free(v->items);
    v->items = NULL;
    v->cap = 0;
    v->len = 0;
}

int vec_push(struct vec *v, char *s) {
    if (v->len == v->cap) {
        size_t new_cap = v->cap == 0 ? 4 : v->cap * 2;
        char **tmp = realloc(v->items, new_cap * sizeof(char *));
        if (tmp == NULL) return ERR_NOMEM;

        v->items = tmp;
        v->cap = new_cap;
    }
    v->items[v->len++] = s;

    return OK;
}
