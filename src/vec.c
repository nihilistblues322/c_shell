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
    fprintf(stderr, "[dbg] vec '%s': len=%zu cap=%zu items=%p\n", label, v->len,
            v->cap, (void *)v->items);
    for (size_t i = 0; i < v->len; i++) {
        if (v->items[i] == NULL)
            fprintf(stderr, "[dbg]   [%zu] NULL (terminator for execv)\n", i);
        else
            fprintf(stderr, "[dbg]   [%zu] %p \"%s\"\n", i, (void *)v->items[i],
                    v->items[i]);
    }
    if (v->cap > v->len)
        fprintf(stderr, "[dbg]   ... %zu free slot(s)\n", v->cap - v->len);
}
#endif

void vec_free(struct vec *v) {
    LOG("vec_free: free items array=%p (len=%zu cap=%zu); the strings are NOT freed here\n",
        (void *)v->items, v->len, v->cap);
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

        LOG("vec_push: array full, grow cap %zu -> %zu, block %s\n", v->cap, new_cap,
            v->items == NULL ? "allocated for the first time"
            : tmp == v->items ? "grown in place" : "moved (realloc copied it)");
        v->items = tmp;
        v->cap = new_cap;
    }
    v->items[v->len++] = s;

    return OK;
}

void vec_free_all(struct vec *v) {
    LOG("vec_free_all: free() each of %zu string(s), then the array\n", v->len);
    for (size_t i = 0; i < v->len; i++) {
        free(v->items[i]);
    }
    vec_free(v);
}
