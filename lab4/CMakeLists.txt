cmake_minimum_required(VERSION 3.22)
project(os)

add_library(Lib1 SHARED
        src/der1.c
        src/bubble_sort.c
)

add_library(Lib2 SHARED
        src/der2.c
        src/quick_sort.c
)

target_link_libraries(Lib1 m)
target_link_libraries(Lib2 m)

add_executable(prog_static
        src/prog_static.c)

target_link_libraries(prog_static Lib1)

add_executable(prog_dynamic
        src/prog_dynamic.c)