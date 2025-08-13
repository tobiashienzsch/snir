#!/bin/bash

set -euxo pipefail

BASE_DIR=$PWD
BUILD_DIR=cmake-build-coverage-clang

export CC="clang-20"
export CXX="clang++-20"

export CXXFLAGS="-march=native -stdlib=libc++ -fprofile-instr-generate -fcoverage-mapping"
export CMAKE_BUILD_TYPE="Debug"
export CMAKE_GENERATOR="Ninja"
export LLVM_PROFILE_FILE="$BUILD_DIR/raw/%p-%m.profraw"

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR/raw"

cmake -S . -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" --target all
ctest --test-dir "$BUILD_DIR" --output-on-failure -j $(nproc)

llvm-profdata-20 merge -sparse $BUILD_DIR/raw/*.profraw -o "$BUILD_DIR/combined.profdata"
llvm-cov-20 show \
  -object $BUILD_DIR/bin/snir-lang \
  -object $BUILD_DIR/bin/snir-opt \
  -object $BUILD_DIR/bin/snir-test-graph \
  -object $BUILD_DIR/bin/snir-test-parser \
  -object $BUILD_DIR/bin/snir-test-interpreter \
  -object $BUILD_DIR/bin/snir-test-v4 \
  -object $BUILD_DIR/bin/snir-test-vector \
  -instr-profile="$BUILD_DIR/combined.profdata" \
  -format=html \
  -output-dir="$BUILD_DIR/html" \
  -show-expansions \
  -show-line-counts-or-regions \
  -ignore-filename-regex='(^/usr/|/test?/|/_deps/)'


llvm-cov-20 export \
  -object $BUILD_DIR/bin/snir-lang \
  -object $BUILD_DIR/bin/snir-opt \
  -object $BUILD_DIR/bin/snir-test-graph \
  -object $BUILD_DIR/bin/snir-test-parser \
  -object $BUILD_DIR/bin/snir-test-interpreter \
  -object $BUILD_DIR/bin/snir-test-v4 \
  -object $BUILD_DIR/bin/snir-test-vector \
  -instr-profile="$BUILD_DIR/combined.profdata" \
  -format=lcov \
  -ignore-filename-regex='(^/usr/|/test?/|/_deps/)' > "$BUILD_DIR/coverage.info"

genhtml $BUILD_DIR/coverage.info -o $BUILD_DIR/lcov-html --ignore-errors unmapped
