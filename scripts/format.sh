#!/bin/bash

find cloader -type f \( -name "*.h" -o -name "*.cpp" \) -exec clang-format -i {} \;
find examples -type f \( -name "*.h" -o -name "*.cpp" \) -exec clang-format -i {} \;
find tests -type f \( -name "*.h" -o -name "*.cpp" \) -exec clang-format -i {} \;
