cmake_minimum_required(VERSION 3.16)

project(ROBO-MAGAZYN VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -DWITH_CUSTOM_PREFIX=1")
set(TARGET_NAME "main")

set(GLOG_VERSION 0.6.0)
set(JSON_VERSION 3.10.5)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(DEP_DIR ${CMAKE_SOURCE_DIR}/dependencies)

set(glog_DIR ${DEP_DIR}/glog-${GLOG_VERSION}/build)
set(nlohmann_json_DIR ${DEP_DIR}/json-${JSON_VERSION}/build)

file(GLOB sources "${SRC_DIR}/*.cpp")

add_executable(${TARGET_NAME} ${sources})

target_include_directories(${TARGET_NAME} PRIVATE "dependencies")

find_package(glog "${GLOG_VERSION}" REQUIRED)
find_package(nlohmann_json "${JSON_VERSION}" REQUIRED)

target_link_libraries(${TARGET_NAME} nlohmann_json::nlohmann_json)
target_link_libraries(${TARGET_NAME} glog::glog)
