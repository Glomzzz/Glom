#!/usr/bin/env bash
mkdir -p cmake_build
cd cmake_build || exit 1
cmake .. -DBUILD_TESTING=Off  -DCMAKE_CXX_COMPILER="/run/current-system/sw/bin/clang++" || exit 1

make glom_exe
