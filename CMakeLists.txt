cmake_minimum_required(VERSION 3.10)
project(aoc)
set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls"
)

add_subdirectory(2025)
