cmake_minimum_required(VERSION 3.20)

project(Crawler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Data Structures
set(DS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(external/DataStructures)

# GoogleTest
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# Crawler Library
add_library(CrawlerLib
    src/crawler/Frontier.cpp
)

target_include_directories(CrawlerLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(CrawlerLib PUBLIC
    DataStructures
)

# Tests
add_executable(FrontierTests
    tests/test_frontier.cpp
)

target_link_libraries(FrontierTests PRIVATE
    CrawlerLib
    gtest_main
)

gtest_discover_tests(FrontierTests)