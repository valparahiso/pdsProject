cmake_minimum_required(VERSION 3.17)
project(clientPdS)

set(CMAKE_CXX_STANDARD 14)

add_executable(clientPdS main.cpp tcp_client.cpp tcp_client.h user.cpp user.h parameters_exception.cpp parameters_exception.h directory_exception.cpp directory_exception.h file_watcher.cpp file_watcher.h filesystem_utility.cpp filesystem_utility.h hash_utility.cpp hash_utility.h JSON_utility.cpp JSON_utility.h)