cmake_minimum_required(VERSION 3.10)
project(Emulator VERSION 0.1)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE src   src/*.cpp)
file(GLOB_RECURSE include   include/*.h)

add_executable(Emulator ${src})
target_include_directories(Emulator PUBLIC include/)