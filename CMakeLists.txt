cmake_minimum_required(VERSION 3.17)
project(file_mapper)

set(CMAKE_CXX_STANDARD 14)
add_compile_options("/Od")
add_executable(file_mapper main.cpp Allocator.h huge_ptr.h)