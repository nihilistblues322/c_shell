/*
 * Тесты для sbuf (строитель строки: символы добавляются по одному).
 *
 * Запуск:  ./test_sbuf
 * Отладка: gdb ./test_sbuf  ->  break on_fail  ->  run
 *
 * Картинка, которую рисует dump:
 *
 *   data --> [ h ][ i ][ ? ][ ? ]     len=2, cap=4
 *              ^^^^^^^   ^
 *              занято    место под '\0' (его поставит sb_finish)
 *
 * Важное правило: в буфере ВСЕГДА остаётся минимум 1 свободная ячейка
 * под терминатор '\0', иначе sb_finish будет писать за границу блока.
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "sbuf.h"
#include "test_util.h"

static void sb_dump(const struct sbuf *sb, const char *label) {
    printf("     +-- %s: len=%zu cap=%zu data=%p\n", label, sb->len, sb->cap,
           (void *)sb->data);
    for (size_t i = 0; i < sb->cap; i++) {
        printf("     |   [%zu] ", i);
        if (i < sb->len) {
            print_char_visible(sb->data[i]);
            printf("\n");
        } else if (i == sb->len) {
            printf("(здесь встанет '\\0')\n");
        } else {
            printf("(свободно)\n");
        }
    }
    if (sb->cap == 0) printf("     |   (память не выделена)\n");
    printf("     +--\n");
}

static void str_dump(const char *s, const char *label) {
    printf("     +-- %s: адрес=%p, байты:\n     |   ", label, (const void *)s);
    for (size_t i = 0;; i++) {
        print_char_visible(s[i]);
        printf(" ");
        if (s[i] == '\0') break;
    }
    printf("\n     +--\n");
}

static void test_init(void) {
    TEST_BEGIN("init", "после sb_init буфер пуст, память не выделена");

    struct sbuf sb;
    sb_init(&sb);
    sb_dump(&sb, "sb");

    CHECK_EQ(sb.len, 0, "len == 0");
    CHECK_EQ(sb.cap, 0, "cap == 0");
    CHECK_TRUE(sb.data == NULL, "data == NULL");

    sb_free(&sb);
}

static void test_basic(void) {
    TEST_BEGIN("basic", "добавили 'a','b','c' -> sb_finish вернул строку \"abc\"");

    struct sbuf sb;
    sb_init(&sb);

    STEP("sb_push 'a'");
    CHECK_EQ(sb_push(&sb, 'a'), OK, "push вернул OK");
    sb_dump(&sb, "sb");
    STEP("sb_push 'b'");
    sb_push(&sb, 'b');
    STEP("sb_push 'c'");
    sb_push(&sb, 'c');
    sb_dump(&sb, "sb");

    STEP("sb_finish(&sb)");
    char *s = sb_finish(&sb);
    str_dump(s, "результат");

    CHECK_STR(s, "abc", "строка == \"abc\"");
    CHECK_EQ(strlen(s), 3, "strlen == 3 ('\\0' в длину не входит)");
    NOTE("Строка теперь принадлежит ВАМ: её надо освободить через free(s).");

    free(s);
    sb_free(&sb);
}

static void test_growth(void) {
    TEST_BEGIN("growth",
               "рост ёмкости: cap 0 -> 4 -> 8, и всегда есть место под '\\0'");

    struct sbuf sb;
    sb_init(&sb);

    for (int i = 0; i < 8; i++) {
        size_t old_cap = sb.cap;
        sb_push(&sb, (char)('a' + i));
        if (sb.cap != old_cap) {
            STEP("push '%c': cap %zu -> %zu (len=%zu)", 'a' + i, old_cap, sb.cap,
                 sb.len);
        } else {
            STEP("push '%c': cap %zu (len=%zu)", 'a' + i, sb.cap, sb.len);
        }
        CHECK_TRUE(sb.len < sb.cap, "len < cap: ячейка под '\\0' зарезервирована");
    }
    sb_dump(&sb, "после 8 push");

    char *s = sb_finish(&sb);
    CHECK_STR(s, "abcdefgh", "содержимое сохранилось после роста");
    free(s);
    sb_free(&sb);
}

static void test_empty(void) {
    TEST_BEGIN("empty", "finish без единого push возвращает пустую строку \"\", а не NULL");

    struct sbuf sb;
    sb_init(&sb);

    STEP("sb_finish сразу после init");
    char *s = sb_finish(&sb);
    str_dump(s, "результат");

    CHECK_TRUE(s != NULL, "результат не NULL");
    CHECK_STR(s, "", "пустая строка");
    NOTE("Пустая строка — это 1 байт '\\0'. Например, аргумент '' в кавычках.");

    free(s);
    sb_free(&sb);
}

static void test_reuse(void) {
    TEST_BEGIN("reuse",
               "после finish буфер сброшен и готов собирать следующее слово");

    struct sbuf sb;
    sb_init(&sb);

    for (const char *p = "hello"; *p; p++) sb_push(&sb, *p);
    char *w1 = sb_finish(&sb);
    STEP("слово 1 собрано: \"%s\"", w1);
    sb_dump(&sb, "sb после finish");

    CHECK_TRUE(sb.data == NULL, "data == NULL (память ушла вызывающему)");
    CHECK_EQ(sb.len, 0, "len == 0");
    CHECK_EQ(sb.cap, 0, "cap == 0");

    for (const char *p = "world"; *p; p++) sb_push(&sb, *p);
    char *w2 = sb_finish(&sb);
    STEP("слово 2 собрано: \"%s\"", w2);

    CHECK_STR(w1, "hello", "первое слово не затёрто вторым");
    CHECK_STR(w2, "world", "второе слово верное");
    CHECK_TRUE(w1 != w2, "это два разных блока памяти");
    NOTE("Именно так токенайзер будет собирать слова команды одно за другим.");

    free(w1);
    free(w2);
    sb_free(&sb);
}

static void test_special_chars(void) {
    TEST_BEGIN("special_chars",
               "пробелы и кавычки внутри слова — обычные символы (нужно для 'a b')");

    struct sbuf sb;
    sb_init(&sb);

    const char *src = "it's a \"test\"";
    for (const char *p = src; *p; p++) sb_push(&sb, *p);
    char *s = sb_finish(&sb);
    str_dump(s, "результат");

    CHECK_STR(s, src, "пробелы и кавычки сохранены как есть");
    NOTE("Это отличие от strtok: strtok режет по пробелам, а sbuf собирает слово целиком.");

    free(s);
    sb_free(&sb);
}

static void test_long(void) {
    TEST_BEGIN("long", "1000 символов подряд: многократный рост без потерь");

    struct sbuf sb;
    sb_init(&sb);

    for (int i = 0; i < 1000; i++) sb_push(&sb, (char)('a' + i % 26));
    CHECK_EQ(sb.len, 1000, "len == 1000");
    CHECK_TRUE(sb.cap > sb.len, "cap > len (есть место под '\\0')");

    char *s = sb_finish(&sb);
    CHECK_EQ(strlen(s), 1000, "strlen == 1000");
    CHECK_TRUE(s[0] == 'a' && s[25] == 'z' && s[26] == 'a' && s[999] == 'l',
               "символы на нужных позициях");

    free(s);
    sb_free(&sb);
}

static void test_free(void) {
    TEST_BEGIN("free", "sb_free освобождает буфер; после finish и повторно — безопасно");

    struct sbuf sb;
    sb_init(&sb);
    sb_push(&sb, 'x');

    STEP("sb_free без finish (слово не доделали)");
    sb_free(&sb);
    CHECK_TRUE(sb.data == NULL, "data == NULL");
    CHECK_EQ(sb.len, 0, "len == 0");

    STEP("sb_free второй раз");
    sb_free(&sb);
    CHECK_TRUE(sb.data == NULL, "повторный free безопасен");

    STEP("finish, затем sb_free: строку уже забрал вызывающий");
    sb_init(&sb);
    sb_push(&sb, 'y');
    char *s = sb_finish(&sb);
    sb_free(&sb);
    CHECK_STR(s, "y", "строка жива после sb_free (она не принадлежит sbuf)");
    free(s);
}

int main(void) {
    setbuf(stdout, NULL);
    printf("##### SBUF TESTS #####\n");

    test_init();
    test_basic();
    test_growth();
    test_empty();
    test_reuse();
    test_special_chars();
    test_long();
    test_free();

    return test_summary("sbuf");
}
