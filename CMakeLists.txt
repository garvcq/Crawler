cmake_minimum_required(VERSION 3.20)

project(Crawler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
find_package(CURL REQUIRED)
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
    src/parser/URLNormalizer.cpp
    src/parser/URLParser.cpp
)

target_include_directories(CrawlerLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(CrawlerLib PUBLIC
    DataStructures
    CURL::libcurl
)

# Tests
#FRONTIER
add_executable(FrontierTests
    tests/test_frontier.cpp
)

target_link_libraries(FrontierTests PRIVATE
    CrawlerLib
    gtest_main
)

gtest_discover_tests(FrontierTests)

#URL NORMALIZER
add_executable(URLNormalizerTests
    tests/normalizer/test_normalize.cpp
    tests/normalizer/test_relativeresolve.cpp
)

target_link_libraries(URLNormalizerTests PRIVATE
    CrawlerLib
    gtest_main
    
)
gtest_discover_tests(URLNormalizerTests)

#URL PARSER
add_executable(URLParserTests
    tests/urlparser/test_general.cpp
    tests/urlparser/test_invalid.cpp
)

target_link_libraries(URLParserTests PRIVATE
    CrawlerLib
    gtest_main
    
)
gtest_discover_tests(URLParserTests)