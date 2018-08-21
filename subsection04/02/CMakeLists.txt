cmake_minimum_required(VERSION 3.5)
project(Socket)

set(CMAKE_CXX_STANDARD 11)

add_executable(SocketServer.out SocketServer.cpp Common.h)
add_executable(SocketClient.out SocketClient.cpp Common.h)
