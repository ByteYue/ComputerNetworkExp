cmake_minimum_required(VERSION 3.18)
project(socket)

set(CMAKE_CXX_STANDARD 14)

add_executable(socket Run.cpp Config.cpp Server.cpp SocketInfo.cpp WinsockEnv.cpp)