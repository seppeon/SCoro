cmake_minimum_required (VERSION 3.16)
project (SCoroTest LANGUAGES CXX)

add_compile_options("/await")

include_directories(${CMAKE_HOME_DIRECTORY})

add_executable(SCoroGen generator.cpp)
add_executable(SCoroTim timing.cpp)
add_executable(SCoroEvn event.cpp)
add_executable(SCoroCom composable.cpp)
add_executable(SCoroCoro coroutine_cpp20.cpp)
add_executable(SCoroThread swap_threads.cpp)

set_property(TARGET SCoroGen PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroTim PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroEvn PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroCom PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroCoro PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroThread PROPERTY CXX_STANDARD 20)