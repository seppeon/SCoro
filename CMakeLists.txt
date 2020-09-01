cmake_minimum_required (VERSION 3.16)
project (SCoro LANGUAGES CXX)
add_executable(SCoro main.cpp)
set_property(TARGET SCoro PROPERTY CXX_STANDARD 20)