cmake_minimum_required(VERSION 3.31.7)
project(Glom)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DCMAKE_C_SCAN_FOR_MODULES ON)
set(DCMAKE_CXX_SCAN_FOR_MODULES ON)


include_directories(include)

add_subdirectory(src)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()