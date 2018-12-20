#!/bin/sh
export LC_MESSAGES=C
cd "$(dirname "$0")"
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -G Ninja ..
ninja