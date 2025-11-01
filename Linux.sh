#!/bin/bash

# output directory

target="Targets/$1"

# generate cmake build files

cmake -S . -B $target -DCMAKE_BUILD_TYPE=$1 -DBUILD_UNIT_TESTS=OFF \
  -DBUILD_EXTRAS=OFF \
  -DBUILD_SHARED_LIBS=ON \
  -DBUILD_BULLET2_DEMOS=OFF \
  -DBUILD_CPU_DEMOS=OFF \
  -DBUILD_OPENGL3_DEMOS=OFF \
  -DUSE_DOUBLE_PRECISION=ON \
  -DBT_USE_DOUBLE_PRECISION=ON

# compile cmake build files

cmake --build $target --config $1

cp "$target/compile_commands.json" . 
