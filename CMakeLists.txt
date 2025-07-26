cmake_minimum_required(VERSION 3.10)
project(HFTSystem)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(hft_system
    src/main.cpp
    src/market_data.cpp
    src/thread_affinity.cpp
)
target_include_directories(hft_system PRIVATE src)
target_link_libraries(hft_system PRIVATE Threads::Threads numa)

add_executable(benchmark
    src/benchmark.cpp
    src/thread_affinity.cpp
)
target_include_directories(benchmark PRIVATE src)
target_link_libraries(benchmark PRIVATE Threads::Threads numa)