cmake_minimum_required(VERSION 3.5)
project(Thread)

set(CMAKE_CXX_STANDARD 11)
find_package(Threads REQUIRED)

add_executable(pThread.out pThread.cpp)
target_link_libraries(pThread.out Threads::Threads)

add_executable(C11Thread.out C11Thread.cpp)
target_link_libraries(C11Thread.out Threads::Threads)