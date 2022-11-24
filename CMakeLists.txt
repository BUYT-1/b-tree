cmake_minimum_required(VERSION 3.24)
project(BTree)

set(CMAKE_CXX_STANDARD 20)

add_executable(BTree_run main.cpp)
include_directories(b_tree)
add_subdirectory(tests)
