cmake_minimum_required(VERSION 2.8)

project(skeleton C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

add_definitions(-DDATA_PATH="data/")

add_subdirectory(libs)
add_subdirectory(src)
