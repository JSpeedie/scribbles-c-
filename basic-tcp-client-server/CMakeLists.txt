cmake_minimum_required(VERSION 3.16)

project(basic-tcp-client-server)
add_executable(basic-tcp-server server.cpp)
add_executable(basic-tcp-client client.cpp)
target_include_directories(basic-tcp-server PUBLIC ".")
target_include_directories(basic-tcp-client PUBLIC ".")
# Uncomment the line below for CMake to compile with debug flags
# set(CMAKE_CXX_FLAGS "-ggdb3")
