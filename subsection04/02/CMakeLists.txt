cmake_minimum_required(VERSION 3.5)
project(TCP_connect)

set(CMAKE_CXX_STANDARD 11)

add_executable(TCP_server.out TCP_server.cpp Common.h)
add_executable(TCP_client.out TCP_client.cpp Common.h)
