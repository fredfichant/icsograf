#!/bin/bash
# script for formatting code with clang-format
find src -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
