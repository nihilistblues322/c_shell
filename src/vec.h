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


#endif
