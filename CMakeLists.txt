cmake_minimum_required(VERSION 3.16)

project(NDArray LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent)

FetchContent_Declare(
googletest
URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(ndarray_tests
tests/tests.cpp
)

target_include_directories(ndarray_tests PRIVATE include)

target_link_libraries(ndarray_tests
GTest::gtest_main
)

target_compile_options(ndarray_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

include(GoogleTest)

gtest_discover_tests(ndarray_tests)
