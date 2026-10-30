#ifndef TEST_UTIL_H
#define TEST_UTIL_H

/*
 * Мини-фреймворк для учебных тестов. Один файл, без зависимостей.
 *
 * Как читать вывод:
 *   === TEST ===   название теста и что он проверяет
 *   ->             шаг: что сейчас делает тест
 *   [PASS]/[FAIL]  результат проверки (ожидание vs факт)
 *
 * Как отлаживать:
 *   gdb ./test_vec
 *   (gdb) break on_fail        <- остановится на первой же упавшей проверке
 *   (gdb) run
 *   (gdb) bt                   <- стек: из какого теста пришли
 */

#include <stdio.h>
#include <string.h>

static int g_checks = 0;
static int g_failed = 0;

/* Точка останова для отладчика: вызывается при каждом [FAIL]. */
__attribute__((noinline)) static void on_fail(void) {
    __asm__ volatile("");
}

#define TEST_BEGIN(name, what)                       \
    do {                                             \
        printf("\n=== TEST: %s ===\n", name);        \
        printf("    Идея: %s\n", what);              \
    } while (0)

#define STEP(...)                \
    do {                         \
        printf("  -> ");          \
        printf(__VA_ARGS__);     \
        printf("\n");            \
    } while (0)

#define NOTE(...)                \
    do {                         \
        printf("     # ");       \
        printf(__VA_ARGS__);     \
        printf("\n");            \
    } while (0)

#define CHECK_TRUE(cond, what)                                              \
    do {                                                                    \
        g_checks++;                                                         \
        if (cond) {                                                         \
            printf("     [PASS] %s\n", what);                               \
        } else {                                                            \
            g_failed++;                                                     \
            printf("     [FAIL] %s\n            условие: %s\n"              \
                   "            место:   %s:%d\n",                          \
                   what, #cond, __FILE__, __LINE__);                        \
            on_fail();                                                      \
        }                                                                   \
    } while (0)

#define CHECK_EQ(actual, expected, what)                                    \
    do {                                                                    \
        unsigned long long a_ = (unsigned long long)(actual);               \
        unsigned long long e_ = (unsigned long long)(expected);             \
        g_checks++;                                                         \
        if (a_ == e_) {                                                     \
            printf("     [PASS] %s (= %llu)\n", what, a_);                  \
        } else {                                                            \
            g_failed++;                                                     \
            printf("     [FAIL] %s\n            ожидали: %llu\n"            \
                   "            получили: %llu\n"                           \
                   "            место:    %s:%d\n",                         \
                   what, e_, a_, __FILE__, __LINE__);                       \
            on_fail();                                                      \
        }                                                                   \
    } while (0)

#define CHECK_STR(actual, expected, what)                                   \
    do {                                                                    \
        const char *a_ = (actual);                                          \
        const char *e_ = (expected);                                        \
        g_checks++;                                                         \
        if (a_ != NULL && e_ != NULL && strcmp(a_, e_) == 0) {              \
            printf("     [PASS] %s (= \"%s\")\n", what, a_);                \
        } else {                                                            \
            g_failed++;                                                     \
            printf("     [FAIL] %s\n            ожидали: \"%s\"\n"          \
                   "            получили: \"%s\"\n"                         \
                   "            место:    %s:%d\n",                         \
                   what, e_ ? e_ : "(NULL)", a_ ? a_ : "(NULL)",            \
                   __FILE__, __LINE__);                                     \
            on_fail();                                                      \
        }                                                                   \
    } while (0)

/* Печать символа так, чтобы '\0', пробел и т. п. были видны. */
__attribute__((unused)) static void print_char_visible(char c) {
    if (c == '\0')      printf("'\\0'");
    else if (c == ' ')  printf("' '  ");
    else if (c == '\n') printf("'\\n'");
    else if (c >= 32 && c < 127) printf("'%c'  ", c);
    else                printf("0x%02x ", (unsigned char)c);
}

static int test_summary(const char *suite) {
    printf("\n--------------------------------------------\n");
    printf("%s: проверок %d, провалено %d  ->  %s\n", suite, g_checks, g_failed,
           g_failed == 0 ? "ВСЁ ОК" : "ЕСТЬ ОШИБКИ");
    return g_failed == 0 ? 0 : 1;
}

#endif
