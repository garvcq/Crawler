cmake_minimum_required(VERSION 3.20)

project(Crawler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
find_package(CURL REQUIRED)
find_path(MARIADB_INCLUDE_DIR
    NAMES mysql/mysql.h
    PATHS C:/msys64/ucrt64/include
)
find_library(MARIADB_LIBRARY
    NAMES mariadb
    PATHS C:/msys64/ucrt64/lib
)


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
    src/crawler/SeenStore.cpp
    src/crawler/Crawler.cpp
    src/parser/URLNormalizer.cpp
    src/parser/URLParser.cpp
    src/parser/HTMLParser.cpp
    src/common/PatternMatcher.cpp
    src/filter/QuickFilter.cpp
    src/filter/LinkFilter.cpp
    src/browser/browserrenderer.cpp
    src/browser/CDPconn.cpp
    src/browser/HTTPClient.cpp
    src/browser/WebSocketConn.cpp
    src/browser/ChromeProcess.cpp
    src/network/HTTPFetcher.cpp
    src/network/PageFetcher.cpp
    src/Storage/MySQLStorage.cpp
    src/config/ConfigLoader.cpp
)

target_include_directories(CrawlerLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MARIADB_INCLUDE_DIR}
)

target_link_libraries(CrawlerLib PUBLIC
    DataStructures
    CURL::libcurl
    ${MARIADB_LIBRARY}
    ws2_32
    winhttp
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

#KMP
add_executable(KMPTests
    tests/patternmatcher/test_basic.cpp
)

target_link_libraries(KMPTests PRIVATE
    CrawlerLib
    gtest_main
    
)
gtest_discover_tests(KMPTests)

#HTML PARSER
add_executable(HTMLTests
    tests/htmlparser/test.cpp
    tests/htmlparser/realtest.cpp
)

target_link_libraries(HTMLTests PRIVATE
    CrawlerLib
    gtest_main
    
)
gtest_discover_tests(HTMLTests)

#LINK FILTER
add_executable(FILTERTests
    tests/linkfilter/test.cpp
    tests/quickfilter/test.cpp
)

target_link_libraries(FILTERTests PRIVATE
    CrawlerLib
    gtest_main
)

gtest_discover_tests(FILTERTests)

#SEENSTORE TESTS
add_executable(SEENSTORETests
    tests/seenstore/test.cpp
)

target_link_libraries(SEENSTORETests PRIVATE
    CrawlerLib
    gtest_main
)

gtest_discover_tests(SEENSTORETests)

#RENDER TESTS
add_executable(RENDERTests
    tests/render/test.cpp
)

target_link_libraries(RENDERTests PRIVATE
    CrawlerLib
    gtest_main
    ws2_32
    winhttp
)

gtest_discover_tests(RENDERTests)

#PAGE FALLBACK TESTS
add_executable(FALLBACKTests
    tests/network/httptest.cpp
    tests/network/pagetest.cpp
)

target_link_libraries(FALLBACKTests PRIVATE
    CrawlerLib
    gtest_main
)

gtest_discover_tests(FALLBACKTests)

#MYSQL TESTS
add_executable(MySQLTest
    tests/database/mysqltest.cpp
)

target_link_libraries(MySQLTest PRIVATE
    CrawlerLib
)

#CONFIGLOADER TESTS
add_executable(ConfigLoaderTests
    tests/config/test_config.cpp
)

target_link_libraries(ConfigLoaderTests PRIVATE
    CrawlerLib
    gtest_main
)

gtest_discover_tests(ConfigLoaderTests)

# INDEXER TESTS
add_executable(IndexerTests
    tests/indexer/test_tokenizer.cpp
    tests/indexer/test_stopwordfilter.cpp
    tests/indexer/test_textextractor.cpp
)

target_link_libraries(IndexerTests PRIVATE
    IndexerLib
    CrawlerLib
    gtest_main
)

gtest_discover_tests(IndexerTests)


#main
add_executable(main
    src/main.cpp
)

target_link_libraries(main PRIVATE
    CrawlerLib
)

# Indexer Library
add_library(IndexerLib
    src/indexer/IndexerStorage.cpp
    src/indexer/TextExtractor.cpp
    src/indexer/Tokenizer.cpp
    src/indexer/StopwordFilter.cpp
    src/indexer/Indexer.cpp
)

target_include_directories(IndexerLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MARIADB_INCLUDE_DIR}
)

target_link_libraries(IndexerLib PUBLIC
    CrawlerLib
    DataStructures
)

# Indexer Executable
add_executable(indexer
    src/indexer/main.cpp
)
target_link_libraries(indexer PRIVATE
    IndexerLib
)

# Search Executable
add_executable(search
    src/search/main.cpp
)
target_link_libraries(search PRIVATE
    IndexerLib
    CrawlerLib
    DataStructures
)

# to copy config into build
file(COPY
    ${CMAKE_SOURCE_DIR}/config
    DESTINATION ${CMAKE_BINARY_DIR}
)