#!/bin/bash

TEST_DIR=test
BUILD_DIR=build

rm -rf "$BUILD_DIR" || exit 1

build_and_test_dir() {
    what=$1
    flags="${@:2}"
    echo "Compiling $what..."
    mkdir -p "$BUILD_DIR/$what" || exit 1
    SESS_DIR=$(mktemp -d -p "$BUILD_DIR/${what}" "XXXXXX")
    if [ "$SESS_DIR" == "" ] ; then
        exit 1
    fi
    for test_cpp in "$TEST_DIR/$what"/*.cpp ; do
        test_name=$(basename -- "$test_cpp")
        test_name=${test_name%.*}
        test_out="$SESS_DIR/${test_name}.o"
        echo "$test_cpp -> $test_out"
        $CXX $test_cpp $CXXFLAGS $COVFLAGS $flags -fopenmp -c -o "${test_out}" || exit 1
    done
    echo "Linking..."
    $CXX "$SESS_DIR"/*.o $COVFLAGS $LDFLAGS -fopenmp -o "$SESS_DIR/test" || exit 1
    (cd "$SESS_DIR" && ./test) || exit 1
}

fail_build_dir() {
    what=$1
    key=$2
    how=$3
    flags="${@:4}"
    echo "Compiling $what..."
    mkdir -p "$BUILD_DIR/$what" || exit 1
    SESS_DIR=$(mktemp -d -p "$BUILD_DIR/${what}" "XXXXXX")
    if [ "$SESS_DIR" == "" ] ; then
        exit 1
    fi
    for test_cpp in "$TEST_DIR/$what"/*.cpp ; do
        test_pre=${test_cpp}.pre
        $CXX $test_cpp $CXXFLAGS $flags -fopenmp -E >$test_pre || exit 1
        NFAIL=$(cat $test_pre | grep "$key" | wc -l)
        echo $test_cpp $NFAIL...
        if [ "$NFAIL" == "" ] ; then
            echo "Fail to grep $test_cpp!"
            exit 1
        fi
        if [ "$NFAIL" == "0" ] ; then
            continue
        fi
        test_name=$(basename -- "$test_cpp")
        test_name=${test_name%.*}
        test_out="$SESS_DIR/${test_name}.o"
        echo "$test_cpp -> $test_out"
        $CXX $test_cpp $CXXFLAGS $flags -fopenmp -c -o "${test_out}" &>build.log
        TFAIL=$(cat build.log | grep "$how" | wc -l)
        if [ "$NFAIL" != "$TFAIL" ] ; then
            echo "$NFAIL != $TFAIL!"
            cat build.log
            exit 1
        fi
    done
}

build_and_test_dir path_static_1 "-DS1B_USE_STATIC_STRINGS"
build_and_test_dir path_static_2 "-DS1B_USE_STATIC_STRINGS"
build_and_test_dir path_static_3 "-DS1B_USE_STATIC_STRINGS"
build_and_test_dir path_dynamic
build_and_test_dir push_buffer_10M
build_and_test_dir push_buffer_16M
build_and_test_dir .
build_and_test_dir . "-DTEST_MOVE"

fail_build_dir . "(original_" "is private" "-DTEST_COPY"
