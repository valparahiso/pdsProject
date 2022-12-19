cmake_minimum_required(VERSION 3.16)
project(serverPdS)

set(CMAKE_CXX_STANDARD 14)

add_executable(serverPdS main.cpp tcp_connection.cpp tcp_server.cpp)