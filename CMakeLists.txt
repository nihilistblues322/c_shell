cmake_minimum_required(VERSION 3.13)

project(shell_learn C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # compile_commands.json для clangd/CLion

add_compile_options(-Wall -Wextra -g)
add_compile_options(-fsanitize=address,undefined)
add_link_options(-fsanitize=address,undefined)
add_compile_definitions($<$<CONFIG:Debug>:DEBUG>) # включает LOG()/VEC_DUMP() только в Debug

# CONFIGURE_DEPENDS: новый .c в src/ подхватится при следующей сборке без ручного reload
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.c)
add_executable(shell ${SOURCE_FILES})

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

    add_executable(test_tokenize tests/test_tokenize.c src/tokenize.c src/vec.c src/sbuf.c)
    target_include_directories(test_tokenize PRIVATE src tests)
    add_test(NAME tokenize COMMAND test_tokenize)
endif ()
