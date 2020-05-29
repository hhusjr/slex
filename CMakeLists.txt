cmake_minimum_required(VERSION 3.15)
project(slex C)

set(CMAKE_C_STANDARD 99)

add_library(nfa_to_min_dfa fa.c fa.h)
add_library(regexp regexp.c regexp.h)
add_library(scanner scanner.c scanner.h)
add_library(slex slex.c slex.h)

add_executable(test test.c)
target_link_libraries(test nfa_to_min_dfa regexp slex scanner)
