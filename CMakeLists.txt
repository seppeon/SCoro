cmake_minimum_required (VERSION 3.16)
project (SCoro LANGUAGES CXX)
add_subdirectory(examples)

add_library(SCoro INTERFACE)
target_include_directories(SCoro INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/inc")