cmake_minimum_required(VERSION 3.23)
project(SimpleDJ)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_subdirectory(libraries)
add_subdirectory(src)
