# shell_learn

Учебный shell на C (изучаем процессы, fork/exec, память).

## Структура
| Файл | Назначение |
|---|---|
| `src/main.c` | цикл: прочитать строку -> разобрать -> выполнить |
| `src/tokenize.c` | разбор строки на слова (кавычки) |
| `src/builtin.c` | встроенные команды: echo, exit, type, cd, pwd |
| `src/exec.c` | поиск в PATH, запуск внешних программ (fork + execv) |
| `src/vec.c`, `src/sbuf.c` | динамический массив строк и строитель строки |
| `src/common.h` | константы, коды ошибок, `LOG` |
| `tests/` | учебные тесты для vec, sbuf, tokenize |

## Сборка и запуск
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/shell
```

## Тесты
```bash
cd build && ctest --output-on-failure     # все
./test_tokenize                            # один, с подробным выводом
```

## Отладка
- Debug-сборка печатает `[dbg] ...` в stderr (скрыть: `./build/shell 2>/dev/null`).
  Release (`-DCMAKE_BUILD_TYPE=Release`) ничего не печатает.
- AddressSanitizer + UBSan включены всегда: ошибки памяти и утечки видны при выходе.
- gdb: `gdb ./build/shell`, для дочерних процессов `set follow-fork-mode child`.
- Упавшая проверка в тестах: `gdb ./build/test_tokenize` -> `break on_fail` -> `run`.
