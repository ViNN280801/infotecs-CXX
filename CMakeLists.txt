cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(infotecs
    VERSION 1.0
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-std=c++20 -Wall -Wpedantic -Wextra")

find_package(Boost 1.76.0 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/headers)

add_executable(client src/Client/client.cpp headers/strmanip.hpp headers/client.hpp headers/server.hpp headers/server_settings.hpp headers/TCP_Connection.hpp)
add_executable(server src/Server/server.cpp headers/strmanip.hpp headers/client.hpp headers/server.hpp headers/server_settings.hpp headers/TCP_Connection.hpp)

set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src/Client")
set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src/Server")
