#!/bin/bash

BUILD_TYPE=${1:-Debug}

cmake -S . -B build                 \
    -DCLOADER_BUILD_EXAMPLES=ON     \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE

cmake --build build --parallel
