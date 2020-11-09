cmake_minimum_required(VERSION 3.17)
project(utilities)

set(CMAKE_CXX_STANDARD 14)

add_library(utilities JSON_utility.cpp JSON_utility.h hash_utility.cpp hash_utility.h filesystem_utility.cpp filesystem_utility.h)