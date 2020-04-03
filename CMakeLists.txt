cmake_minimum_required(VERSION 3.13)
project(compilator C)

set(CMAKE_C_STANDARD 11)

add_library(hashtable libs/hashtable.c libs/list.c libs/common.c includes/hashtable.h includes/list.h includes/common.h libs/lexem_analyser.c includes/lexem_analyser.h libs/syntax_analyser.c includes/syntax_analyser.h libs/tree.c includes/tree.h libs/translator.c includes/translator.h)

link_directories(libs)
include_directories(includes)


add_executable(compilator main.c libs/translator.c includes/translator.h)

target_link_libraries(compilator hashtable)
