#ifndef VEC_H
#define VEC_H
#include <stddef.h>

struct vec {
    char **items;
    size_t len;
    size_t cap;
};

void vec_init(struct vec *v);

int vec_push(struct vec *v, char *s);

void vec_free(struct vec *v);

void vec_free_all(struct vec *v);

/* Debug: print vec contents to stderr. Compiles to nothing in Release. */
#ifdef DEBUG
void vec_debug_dump(const struct vec *v, const char *label);
#define VEC_DUMP(v, label) vec_debug_dump((v), (label))
#else
#define VEC_DUMP(v, label) ((void)0)
#endif


#endif
