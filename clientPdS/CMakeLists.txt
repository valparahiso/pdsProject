cmake_minimum_required(VERSION 3.17)
project(clientPdS)

set(CMAKE_CXX_STANDARD 14)

add_executable(clientPdS main.cpp tcp_client.cpp tcp_client.h)