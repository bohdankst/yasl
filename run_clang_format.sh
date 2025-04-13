#!/bin/bash

find include -iname '*.h' -o -iname "*.c" -o -iname "*.hpp" -o -iname "*.cpp" | xargs clang-format -i -style=file
find src -iname '*.h' -o -iname "*.c" -o -iname "*.hpp" -o -iname "*.cpp" | xargs clang-format -i -style=file
find ut -iname '*.h' -o -iname "*.c" -o -iname "*.hpp" -o -iname "*.cpp" | xargs clang-format -i -style=file