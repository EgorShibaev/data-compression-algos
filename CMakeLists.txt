cmake_minimum_required(VERSION 2.8)
project(data-compression-algos)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Werror -std=c++17")
include_directories("include/")
set(
        SRC_LIST
        src/bit_manipulating.cpp src/huffman/huffman_coding.cpp src/huffman/huffman_tree.cpp
        src/statistic.cpp src/arithmetic/range.cpp src/arithmetic/bit_buffer.cpp src/arithmetic/arithmetic_coding.cpp
        src/BWT/bwt.cpp src/BWT/rle.cpp src/BWT/bwt_coding.cpp
)

add_executable(dca ${SRC_LIST} src/main.cpp)
add_executable(test ${SRC_LIST} src/test.cpp)