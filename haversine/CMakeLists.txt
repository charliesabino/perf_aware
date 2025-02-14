cmake_minimum_required(VERSION 3.20)
project(haversine)

set(CMAKE_CXX_COMPILER g++-13)
set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} src/main.cpp src/haversine.cpp src/generator.cpp)
