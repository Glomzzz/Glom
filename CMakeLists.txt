cmake_minimum_required(VERSION 3.31.7)
project(Glom)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "/run/current-system/sw/bin/clang++")

include_directories(include)

add_subdirectory(src)
add_subdirectory(tests)
