cmake_minimum_required(VERSION 3.10)
project(Emulator VERSION 0.1)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(NES/src)
add_subdirectory(debugger/src)