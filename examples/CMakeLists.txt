cmake_minimum_required (VERSION 3.16)
project (SCoroTest LANGUAGES CXX)

include_directories(${CMAKE_HOME_DIRECTORY})

add_executable(SCoroGen generator.cpp)
add_executable(SCoroTim timing.cpp)
add_executable(SCoroEvn event.cpp)
add_executable(SCoroCom composable.cpp)
add_executable(SCoroThread swap_threads.cpp)

set_property(TARGET SCoroGen PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroTim PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroEvn PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroCom PROPERTY CXX_STANDARD 20)
set_property(TARGET SCoroThread PROPERTY CXX_STANDARD 20)

if(UNIX)
  target_link_libraries(SCoroThread -lpthread)
  target_link_libraries(SCoroEvn -lpthread)
endif()

if(WIN32)
  add_compile_options("/await") # only for visual studio
  add_executable(SCoroCoro coroutine_cpp20.cpp)
  set_property(TARGET SCoroCoro PROPERTY CXX_STANDARD 20)
endif()
