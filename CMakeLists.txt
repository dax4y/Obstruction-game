cmake_minimum_required(VERSION 3.17)
project(ps6 C)

set(CMAKE_C_STANDARD 11)

find_package(Curses REQUIRED)
link_libraries("-lncurses")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -Wextra -Werror -pedantic")

add_executable(ps6 program.c)
