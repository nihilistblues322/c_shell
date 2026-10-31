cmake_minimum_required(VERSION 3.13)

project(codecrafters-shell)

file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

set(CMAKE_C_STANDARD 23) # Enable the C23 standard

add_compile_options(-Wall -Wextra -g)
add_compile_options(-fsanitize=address,undefined)
add_link_options(-fsanitize=address,undefined)
add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

add_executable(shell ${SOURCE_FILES}
        src/vec.h
        src/vec.c)

target_link_libraries(shell PRIVATE readline)

# --- учебные тесты: ./test_vec, ./test_sbuf или `ctest -V` ---
option(BUILD_TESTS "Build unit tests in tests/" ON)
if (BUILD_TESTS)
    enable_testing()

    add_executable(test_vec tests/test_vec.c src/vec.c)
    target_include_directories(test_vec PRIVATE src tests)
    add_test(NAME vec COMMAND test_vec)

    add_executable(test_sbuf tests/test_sbuf.c src/sbuf.c)
    target_include_directories(test_sbuf PRIVATE src tests)
    add_test(NAME sbuf COMMAND test_sbuf)

    # tokenize живёт в builtin.c, поэтому линкуем его целиком (main.c не нужен)
    add_executable(test_tokenize tests/test_tokenize.c src/builtin.c src/vec.c src/sbuf.c)
    target_include_directories(test_tokenize PRIVATE src tests)
    add_test(NAME tokenize COMMAND test_tokenize)
endif ()
