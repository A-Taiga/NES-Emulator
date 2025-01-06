cmake_minimum_required(VERSION 3.10)
project(Emulator VERSION 0.1)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

add_subdirectory(NES/src)
add_subdirectory(debugger/src)
add_subdirectory(mos6502/src)