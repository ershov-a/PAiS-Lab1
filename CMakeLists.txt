cmake_minimum_required(VERSION 3.21)
project(MCS_Lock)

set(CMAKE_CXX_STANDARD 14)
link_libraries("-static")
add_executable(MCS_Lock mcs_test.cpp)
