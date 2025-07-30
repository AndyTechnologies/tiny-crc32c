cmake_minimum_required(VERSION 3.16)
project(tiny_crc32c_benchmark)

set(CMAKE_CXX_STANDARD 23)

add_executable(tiny_crc32c_benchmark benchmark.cpp)
target_include_directories(tiny_crc32c_benchmark PRIVATE ../include)
target_compile_definitions(tiny_crc32c_benchmark PRIVATE _IMPL_TINY_CRC32_)