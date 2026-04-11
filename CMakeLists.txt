cmake_minimum_required(VERSION 3.26)
project(lab1_matrix_char_null C)

set(CMAKE_C_STANDARD 11)

add_executable(lab1_matrix_char_null
        main.c
        matrix.c
        field_info.c
        tests.c
)
