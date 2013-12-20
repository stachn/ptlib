cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME "hello_world")
project(${PROJECT_NAME} C CXX)

file(GLOB SRC_FILES *.cxx)

include_directories(${PTLIB_INCLUDE_PATH})

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PTLibStatic)


