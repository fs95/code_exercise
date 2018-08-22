cmake_minimum_required(VERSION 3.5)
project(Socket)

set(CMAKE_CXX_STANDARD 11)
find_package(Threads REQUIRED)

add_executable(SocketServer.out SocketServer.cpp SocketCommon.h GetDoubleTime.cpp GetDoubleTime.h SocketCommon.cpp)
target_link_libraries(SocketServer.out Threads::Threads)

add_executable(SocketClient.out SocketClient.cpp SocketCommon.h GetDoubleTime.cpp GetDoubleTime.h SocketCommon.cpp)
target_link_libraries(SocketClient.out Threads::Threads)
