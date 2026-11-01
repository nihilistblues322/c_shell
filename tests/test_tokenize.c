/*
 * Один базовый тест для tokenize(const char *line, struct vec *out).
 *
 * Контракт:
 *   - слова режутся по пробелам, лишние пробелы игнорируются;
 *   - '...' — всё внутри буквально (пробелы не режут), кавычки убираются;
 *   - каждый токен — отдельная malloc-строка (освобождает вызывающий);
 *   - NULL-терминатор для execv tokenize НЕ добавляет.
 *
 * Запуск: ./test_tokenize     Отладка: gdb ./test_tokenize -> break on_fail
 */

#include <stdio.h>
#include <stdlib.h>

#include "tokenize.h"
#include "common.h"
#include "test_util.h"
#include "vec.h"

int main(void) {
    setbuf(stdout, NULL);

    const char *line = "  echo   'hello   world'  foo  ";
    const char *expected[] = {"echo", "hello   world", "foo"};

    printf("вход: <%s>\n", line);

    struct vec out;
    vec_init(&out);
    int rc = tokenize(line, &out);

    printf("получили %zu токенов:", out.len);
    for (size_t i = 0; i < out.len; i++) printf(" [%s]", out.items[i]);
    printf("\n");
    printf("ожидали  3 токена:    [echo] [hello   world] [foo]\n\n");

    CHECK_EQ(rc, OK, "tokenize вернул OK");
    CHECK_EQ(out.len, 3, "3 токена (лишние пробелы не дали пустых)");
    for (size_t i = 0; i < out.len && i < 3; i++)
        CHECK_STR(out.items[i], expected[i], "токен");

    for (size_t i = 0; i < out.len; i++) free(out.items[i]);
    vec_free(&out);

    return test_summary("tokenize");
}
