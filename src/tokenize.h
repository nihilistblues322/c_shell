#ifndef TOKENIZE_H
#define TOKENIZE_H

/* Разбор строки команды на слова (учёт кавычек). */

#include "vec.h"

/*
 * Режет line на токены и дописывает их в out.
 * Каждый токен — отдельная строка в куче: освобождает вызывающий
 * (vec_free_all). Возвращает OK, ERR_SYNTAX или ERR_NOMEM.
 */
int tokenize(const char *line, struct vec *out);

#endif
