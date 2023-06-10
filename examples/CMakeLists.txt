cmake_minimum_required (VERSION 3.16)
project (SCoroTest LANGUAGES CXX)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(SCoroGen generator.cpp)
target_link_libraries(SCoroGen PRIVATE SCoro)
target_compile_features(SCoroGen PRIVATE cxx_std_17)

add_executable(SCoroTim timing.cpp)
target_link_libraries(SCoroTim PRIVATE SCoro)
target_compile_features(SCoroTim PRIVATE cxx_std_17)

add_executable(SCoroEvn event.cpp)
target_link_libraries(SCoroEvn PRIVATE SCoro Threads::Threads)
target_compile_features(SCoroEvn PRIVATE cxx_std_17)

add_executable(SCoroCom composable.cpp)
target_link_libraries(SCoroCom PRIVATE SCoro)
target_compile_features(SCoroCom PRIVATE cxx_std_17)

add_executable(SCoroThread swap_threads.cpp)
target_link_libraries(SCoroThread PRIVATE SCoro Threads::Threads)
target_compile_features(SCoroThread PRIVATE cxx_std_17)

add_executable(SCoroCoro coroutine_cpp20.cpp)
target_link_libraries(SCoroCoro PRIVATE SCoro)
target_compile_features(SCoroCoro PRIVATE cxx_std_20)