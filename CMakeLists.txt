cmake_minimum_required(VERSION 2.8)
project(data-compression-algos)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Werror -std=c++17")
include_directories("include/")
set(SRC_LIST src/bit_manipulating.cpp src/huffman_coding.cpp src/huffman_tree.cpp src/statistic.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST} src/main.cpp)
add_executable(test ${SRC_LIST} src/test.cpp)