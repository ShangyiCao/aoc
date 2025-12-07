cmake_minimum_required(VERSION 3.10)
project(aoc)
add_subdirectory(source)
set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls"
)