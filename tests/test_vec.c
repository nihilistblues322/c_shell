/*
 * Тесты для vec (динамический массив указателей на строки).
 *
 * Запуск:  ./test_vec
 * Отладка: gdb ./test_vec  ->  break on_fail  ->  run
 *
 * Картинка, которую рисует dump:
 *
 *   len = сколько элементов реально лежит
 *   cap = сколько ячеек выделено (ёмкость)
 *
 *   items --> [ 0 ][ 1 ][ 2 ][ . ]      len=3, cap=4
 *              ^^^^^^^^^^^^^   ^
 *              занято          свободно
 */

#include <stdio.h>
#include <stdlib.h>

#include "test_util.h"
#include "vec.h"

static void vec_dump(const struct vec *v, const char *label) {
    printf("     +-- %s: len=%zu cap=%zu items=%p\n", label, v->len, v->cap,
           (void *)v->items);
    for (size_t i = 0; i < v->cap; i++) {
        if (i < v->len) {
            if (v->items[i] == NULL)
                printf("     |   [%zu] NULL        <- терминатор для execv\n", i);
            else
                printf("     |   [%zu] %p \"%s\"\n", i, (void *)v->items[i],
                       v->items[i]);
        } else {
            printf("     |   [%zu] (свободно)\n", i);
        }
    }
    if (v->cap == 0) printf("     |   (память не выделена)\n");
    printf("     +--\n");
}

static void test_init(void) {
    TEST_BEGIN("init", "после vec_init вектор пустой и память не выделена");

    struct vec v;
    STEP("vec_init(&v)");
    vec_init(&v);
    vec_dump(&v, "v");

    CHECK_EQ(v.len, 0, "len == 0");
    CHECK_EQ(v.cap, 0, "cap == 0");
    CHECK_TRUE(v.items == NULL, "items == NULL (malloc ещё не вызывали)");

    vec_free(&v);
}

static void test_first_push(void) {
    TEST_BEGIN("first_push",
               "первый push выделяет память: cap 0 -> 4 (realloc(NULL,..) == malloc)");

    struct vec v;
    vec_init(&v);

    STEP("vec_push(&v, \"ls\")");
    int rc = vec_push(&v, "ls");
    vec_dump(&v, "v");

    CHECK_EQ(rc, 0, "push вернул OK (0)");
    CHECK_EQ(v.len, 1, "len == 1");
    CHECK_EQ(v.cap, 4, "cap == 4 (начальная ёмкость)");
    CHECK_STR(v.items[0], "ls", "items[0] == \"ls\"");

    vec_free(&v);
}

static void test_growth(void) {
    TEST_BEGIN("growth",
               "когда len дошёл до cap, ёмкость удваивается, старые элементы сохраняются");

    struct vec v;
    vec_init(&v);
    const char *words[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i"};

    for (int i = 0; i < 9; i++) {
        size_t old_cap = v.cap;
        void *old_ptr = (void *)v.items;
        vec_push(&v, (char *)words[i]);
        if (v.cap != old_cap) {
            const char *how = old_ptr == NULL
                                  ? "память выделена впервые"
                                  : old_ptr == (void *)v.items
                                        ? "блок вырос на месте"
                                        : "блок ПЕРЕЕХАЛ в новое место (realloc скопировал)";
            STEP("push \"%s\": cap %zu -> %zu, %s", words[i], old_cap, v.cap, how);
        } else {
            STEP("push \"%s\": cap остался %zu", words[i], v.cap);
        }
    }
    vec_dump(&v, "после 9 push");

    CHECK_EQ(v.len, 9, "len == 9");
    CHECK_EQ(v.cap, 16, "cap == 16 (4 -> 8 -> 16)");
    int ok = 1;
    for (int i = 0; i < 9; i++)
        if (v.items[i] != words[i]) ok = 0;
    CHECK_TRUE(ok, "все 9 элементов на своих местах после перекопирований");
    NOTE("Если items поменялся адрес — любой старый указатель на v.items[...] "
         "теперь висячий. Поэтому в vec_push нельзя хранить старый адрес.");

    vec_free(&v);
}

static void test_many(void) {
    TEST_BEGIN("many", "1000 push подряд: ёмкость растёт степенями двойки");

    struct vec v;
    vec_init(&v);
    static char buf[1000][8];

    size_t last_cap = 0;
    int grows = 0;
    for (int i = 0; i < 1000; i++) {
        snprintf(buf[i], sizeof(buf[i]), "s%d", i);
        vec_push(&v, buf[i]);
        if (v.cap != last_cap) {
            printf("     cap: %zu -> %zu (на элементе #%d)\n", last_cap, v.cap, i);
            last_cap = v.cap;
            grows++;
        }
    }

    CHECK_EQ(v.len, 1000, "len == 1000");
    CHECK_EQ(v.cap, 1024, "cap == 1024");
    CHECK_EQ(grows, 9, "realloc вызывался 9 раз (4,8,...,1024), а не 1000");
    CHECK_STR(v.items[0], "s0", "первый элемент");
    CHECK_STR(v.items[999], "s999", "последний элемент");

    vec_free(&v);
}

static void test_null_terminator(void) {
    TEST_BEGIN("null_terminator",
               "так собирается argv для execv: имя, аргументы, потом NULL");

    struct vec v;
    vec_init(&v);

    STEP("push \"ls\", \"-l\", \"/tmp\", NULL");
    vec_push(&v, "ls");
    vec_push(&v, "-l");
    vec_push(&v, "/tmp");
    vec_push(&v, NULL);
    vec_dump(&v, "argv");

    CHECK_EQ(v.len, 4, "len == 4 (NULL тоже занимает ячейку)");
    CHECK_TRUE(v.items[3] == NULL, "последний элемент NULL");
    CHECK_STR(v.items[0], "ls", "argv[0] == имя программы");
    NOTE("Число аргументов (argc) = len - 1. execv(path, v.items) пройдёт до NULL.");

    vec_free(&v);
}

static void test_ownership(void) {
    TEST_BEGIN("ownership",
               "vec хранит УКАЗАТЕЛИ, а не копии строк: строкой владеет вызывающий");

    struct vec v;
    vec_init(&v);

    char word[] = "hello";
    STEP("char word[] = \"hello\"; vec_push(&v, word);");
    vec_push(&v, word);

    CHECK_TRUE(v.items[0] == word, "items[0] — тот же адрес, что и word (не копия)");

    STEP("word[0] = 'J';  (меняем исходную строку)");
    word[0] = 'J';
    vec_dump(&v, "v");
    CHECK_STR(v.items[0], "Jello", "вектор «видит» изменение, потому что указывает на ту же память");
    NOTE("Вывод: пока vec жив, строки, на которые он указывает, обязаны жить. "
         "vec_free освобождает только массив items, но не сами строки.");

    vec_free(&v);
}

static void test_free(void) {
    TEST_BEGIN("free", "vec_free обнуляет поля, повторный вызов безопасен");

    struct vec v;
    vec_init(&v);
    vec_push(&v, "x");
    vec_push(&v, "y");

    STEP("vec_free(&v)");
    vec_free(&v);
    vec_dump(&v, "v после free");
    CHECK_TRUE(v.items == NULL, "items == NULL (нет висячего указателя)");
    CHECK_EQ(v.len, 0, "len == 0");
    CHECK_EQ(v.cap, 0, "cap == 0");

    STEP("vec_free(&v) второй раз");
    vec_free(&v);
    CHECK_TRUE(v.items == NULL, "двойной free не упал (free(NULL) допустим)");

    STEP("vec снова можно использовать после free");
    vec_push(&v, "again");
    CHECK_STR(v.items[0], "again", "push после free работает");
    vec_free(&v);
}

int main(void) {
    setbuf(stdout, NULL);
    printf("##### VEC TESTS #####\n");

    test_init();
    test_first_push();
    test_growth();
    test_many();
    test_null_terminator();
    test_ownership();
    test_free();

    return test_summary("vec");
}
