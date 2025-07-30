cmake_minimum_required(VERSION 3.16)
project(crc32c_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

# Añade tu librería
add_library(crc32c INTERFACE)
target_include_directories(crc32c INTERFACE ${CMAKE_SOURCE_DIR}/include)

# Ejecutable de tests
add_executable(tests tests/crc32.cpp)
target_link_libraries(tests PRIVATE crc32c)
# incluye el header de doctest en tests/
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/tests)

enable_testing()
add_test(NAME crc32c_tests COMMAND tests)
