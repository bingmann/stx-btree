cmake_minimum_required(VERSION 3.12)
project(STXBTree)

set(CMAKE_CXX_STANDARD 14)

if(MSVC)
    set(CMAKE_CXX_FLAGS "/O2 /arch:AVX2 /W1 /EHsc")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "-O3 -xHost")
else()
    # clang and gcc
    set(CMAKE_CXX_FLAGS "-O3 -march=native -Wall -Wextra")
endif()

include_directories(include/stx)

add_executable(mvb include/mvb/mvb.cpp)
add_executable(basic include/mvb/basic.cpp)
# add_executable(example include/example/main.cpp)
# add_executable(benchmark include/benchmark/main.cpp)
# add_executable(benchmark_uint64 include/benchmark/main_uint64.cpp)
# add_executable(benchmark_640bit include/benchmark/main_640bit.cpp)