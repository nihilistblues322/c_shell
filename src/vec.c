#include "vec.h"
#include <stdlib.h>
#include "common.h"

void vec_init(struct vec *v) {
    v->len = 0;
    v->cap = 0;
    v->items = NULL;
}

#ifdef DEBUG
void vec_debug_dump(const struct vec *v, const char *label) {
    fprintf(stderr, "[%s] len=%zu cap=%zu:", label, v->len, v->cap);
    for (size_t i = 0; i < v->len; i++) {
        if (v->items[i] == NULL) fprintf(stderr, " NULL");
        else fprintf(stderr, " \"%s\"", v->items[i]);
    }
    fprintf(stderr, "\n");
}
#endif

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

        LOG("[vec] grow cap %zu -> %zu\n", v->cap, new_cap);
        v->items = tmp;
        v->cap = new_cap;
    }
    v->items[v->len++] = s;
    LOG("[vec] push %s%s%s len=%zu cap=%zu\n", s ? "\"" : "", s ? s : "NULL", s ? "\"" : "", v->len,
        v->cap);

    return OK;
}

void vec_free_all(struct vec *v) {
    LOG("[vec] free_all len=%zu\n", v->len);
    for (size_t i = 0; i < v->len; i++) {
        free(v->items[i]);
    }
    vec_free(v);
}
