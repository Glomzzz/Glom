#!/usr/bin/env bash
mkdir -p cmake_test
cd cmake_test || exit 1
cmake .. -DBUILD_TESTING=On -DCMAKE_CXX_COMPILER="/run/current-system/sw/bin/clang++" || exit 1

make test_tokenizer || exit 1
./tests/test_tokenizer || exit 1

make test_parser || exit 1
./tests/test_parser || exit 1