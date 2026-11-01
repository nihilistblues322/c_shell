#include "tokenize.h"

#include "common.h"
#include "sbuf.h"
#include "vec.h"

int tokenize(const char *line, struct vec *out) {
    struct sbuf sb;
    sb_init(&sb);
    int in_quote = 0;
    int in_word = 0;

    LOG("tokenize: input <%s>\n", line);
    for (const char *p = line; *p != '\0'; p++) {
        LOG("tokenize: char '%c' (index %td) in_quote=%d in_word=%d sb=\"%.*s\"\n", *p,
            p - line, in_quote, in_word, (int)sb.len, sb.data ? sb.data : "");

        if (in_quote) {
            if (*p == '\'') {
                in_quote = 0;
            } else {
                if (sb_push(&sb, *p) != OK) return ERR_NOMEM;
            }
        } else if (*p == '\'') {
            in_quote = 1;
            in_word = 1;
        } else if (*p == ' ') {
            if (in_quote == 1) {
                sb_free(&sb);
                return ERR_SYNTAX;
            }
            if (in_word) {
                char *finish = sb_finish(&sb);
                LOG("tokenize: space -> word complete \"%s\" (heap %p), push to out\n", finish,
                    (void *)finish);
                if (vec_push(out, finish) != OK) return ERR_NOMEM;
                in_word = 0;
            }
        } else {
            if (sb_push(&sb, *p) != OK) return ERR_NOMEM;
            in_word = 1;
        }
    }


    if (in_word == 1) {
        char *finish = sb_finish(&sb);
        LOG("tokenize: end of line -> word complete \"%s\" (heap %p), push to out\n", finish,
            (void *)finish);
        if (vec_push(out, finish) != OK) return ERR_NOMEM;
        in_word = 0;
    }

    VEC_DUMP(out, "tokenize result");
    return OK;
}
