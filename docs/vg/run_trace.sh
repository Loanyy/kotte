#!/bin/bash
# Run from the repo root on Linux with g++-14: bash docs/vg/run_trace.sh [counter-a|counter-b|counter-c]
# Without an argument it traces the current code. With one, it traces a copy of the code with that counter patch.
set -e
ROOT=$(pwd)
WORK=$(mktemp -d)
cp -r kotte vendor "$WORK"
if [ -n "$1" ]; then
    (cd "$WORK" && patch -s -p1 < "$ROOT/docs/vg/$1.diff")
fi
cd "$WORK"
SRC=$(ls kotte/src/*.cpp | grep -v main.cpp)
g++-14 -std=c++23 -O1 -Ikotte/include -isystem vendor/raylib/include -isystem vendor/cpp_prngs/include \
    "$ROOT/docs/vg/trace_scenarios.cpp" $SRC "$ROOT/docs/vg/raylib_stub.cpp" -o trace
./trace
