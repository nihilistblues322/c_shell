#include "tokenize.h"

#include <stdlib.h>

#include "common.h"
#include "sbuf.h"
#include "vec.h"

enum {
    NORMAL,
    IN_QUOTE
};

int tokenize(const char *line, struct vec *out) {
    struct sbuf sb;
    sb_init(&sb);

    int state = NORMAL;
    int in_word = 0;

    for (const char *p = line; *p != '\0'; p++) {
        LOG("[tok] i=%td c='%c' state=%d word=%d sb=\"%.*s\"\n",
            p - line, *p, state, in_word,
            (int)sb.len, sb.data ? sb.data : "");

        switch (state) {
            case IN_QUOTE:
                if (*p == '\'') {
                    state = NORMAL;
                } else {
                    if (sb_push(&sb, *p) != OK) {
                        sb_free(&sb);
                        return ERR_NOMEM;
                    }
                }
                break;

            case NORMAL:
                switch (*p) {
                    case '\'':
                        state = IN_QUOTE;
                        in_word = 1;
                        break;

                    case ' ':
                        if (in_word) {
                            char *finish = sb_finish(&sb);
                            if (!finish) {
                                sb_free(&sb);
                                return ERR_NOMEM;
                            }

                            if (vec_push(out, finish) != OK) {
                                free(finish);
                                sb_free(&sb);
                                return ERR_NOMEM;
                            }

                            in_word = 0;
                        }
                        break;

                    default:
                        if (sb_push(&sb, *p) != OK) {
                            sb_free(&sb);
                            return ERR_NOMEM;
                        }
                        in_word = 1;
                        break;
                }
                break;
        }
    }

    if (state == IN_QUOTE) {
        sb_free(&sb);
        return ERR_SYNTAX;
    }

    if (in_word) {
        char *finish = sb_finish(&sb);
        if (!finish) {
            sb_free(&sb);
            return ERR_NOMEM;
        }

        if (vec_push(out, finish) != OK) {
            free(finish);
            sb_free(&sb);
            return ERR_NOMEM;
        }
    }

    return OK;
}
