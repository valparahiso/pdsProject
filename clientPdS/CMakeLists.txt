cmake_minimum_required(VERSION 3.17)
project(clientPdS)

set(CMAKE_CXX_STANDARD 14)

add_executable(clientPdS main.cpp tcp_client.cpp tcp_client.h user.cpp user.h parameters_exception.cpp parameters_exception.h directory_exception.cpp directory_exception.h file_watcher.cpp file_watcher.h utilities/JSON_utility.cpp utilities/JSON_utility.h utilities/hash_utility.cpp utilities/hash_utility.h utilities/filesystem_utility.cpp utilities/filesystem_utility.h)