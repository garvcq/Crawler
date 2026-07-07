cmake_minimum_required(VERSION 3.20)

project(CDPCrawler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CDPCrawler
    src/main.cpp
    src/ChromeProcess.cpp
    src/HTTPClient.cpp
    src/WebSocketConn.cpp
    src/CDPconn.cpp
)

target_include_directories(CDPCrawler
PRIVATE include)

target_link_libraries(CDPCrawler
    PRIVATE
        ws2_32
        winhttp
)