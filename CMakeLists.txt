cmake_minimum_required (VERSION 3.16)
project (SCoro LANGUAGES CXX)

add_compile_options("/await")

add_executable(SCoroGen generator_example.cpp)
add_executable(SCoroTim timing_example.cpp)
add_executable(SCoroEvn event_example.cpp)
add_executable(SCoroCom composable_example.cpp)
add_executable(SCoroCoro coroutine_cpp20_example.cpp)
add_executable(SCoroThread resume_on_other_thread.cpp)

set_property(TARGET SCoroGen PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroTim PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroEvn PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroCom PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroCoro PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroThread PROPERTY CXX_STANDARD 20)