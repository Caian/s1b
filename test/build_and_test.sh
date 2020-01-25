#!/bin/bash

TEST_DIR=test
BUILD_DIR=build

rm -rf "$BUILD_DIR" || exit 1

build_and_test_dir() {
    what=$1
    flags="${@:2}"
    echo "Compiling $what..."
    mkdir -p "$BUILD_DIR/$what" || exit 1
    for test_cpp in "$TEST_DIR/$what"/*.cpp ; do
        test_name=$(basename -- "$test_cpp")
        test_name=${test_name%.*}
        test_out="$BUILD_DIR/$what/${test_name}.o"
        echo "$test_cpp -> $test_out"
        $CXX $test_cpp $CXXFLAGS $COVFLAGS $flags -c -o "${test_out}" || exit 1
    done
    echo "Linking..."
    $CXX "$BUILD_DIR/$what"/*.o $COVFLAGS $LDFLAGS -o "$BUILD_DIR/$what/test" || exit 1
    (cd "$BUILD_DIR/$what" && ./test) || exit 1
}

build_and_test_dir path_static_1 "-DS1B_USE_STATIC_STRINGS"
build_and_test_dir path_static_2 "-DS1B_USE_STATIC_STRINGS"
build_and_test_dir path_static_3 "-DS1B_USE_STATIC_STRINGS"
build_and_test_dir path_dynamic
build_and_test_dir .
