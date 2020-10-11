cmake_minimum_required(VERSION 3.18)
project(socket)

set(CMAKE_CXX_STANDARD 17)

add_executable(socket main.cpp config.cpp server.cpp  fileProcess.cpp)