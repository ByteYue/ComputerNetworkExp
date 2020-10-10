cmake_minimum_required(VERSION 3.18)
project(socket)

set(CMAKE_CXX_STANDARD 14)

add_executable(socket config.cpp server.cpp SocketInfo.cpp cmake-build-debug/client.h client.h)