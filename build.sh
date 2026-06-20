#!/bin/bash

cd build
cmake .. -DCMAKE_PREFIX_PATH=lib -G Ninja -DBUILD_TESTS=ON
cmake --build .
