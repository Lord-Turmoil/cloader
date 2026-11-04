#!/bin/bash

BUILD_TYPE=${1:-Debug}

cmake -S . -B build                 \
    -DCLOADER_BUILD_EXAMPLES=ON     \
    -DCLOADER_BUILD_DOCS=ON         \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE

echo "Building Cloader with build type: $BUILD_TYPE"
cmake --build build --parallel
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

echo "Building docs"
cmake --build build --target docs
