cmake_minimum_required(VERSION 3.22)
project(bookstore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-g -O2")
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(code src/Bookstore.cpp)
