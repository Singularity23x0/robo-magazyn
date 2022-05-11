cmake_minimum_required(VERSION 3.12)

project(ROBO-MAGAZYN VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall")

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(DEP_DIR ${CMAKE_SOURCE_DIR}/dependencies)

file(GLOB sources "${SRC_DIR}/*.cpp")

add_executable(main ${sources})
