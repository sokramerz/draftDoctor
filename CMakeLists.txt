cmake_minimum_required(VERSION 3.14)
project(draft-doctor LANGUAGES CXX)

# Standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. Unit Testing
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

# 2. Performance Metrics
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.8.3
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE) # Speed up build
FetchContent_MakeAvailable(benchmark)

# bundles the algos into a library so Main, Tests, and Benchmarks can all share it.
add_library(miners_lib
    src/apriori.cpp
    src/fpgrowth.cpp
    src/utils.cpp
)
target_include_directories(miners_lib PUBLIC include)

# Main CLI Executable 
add_executable(draft-doctor src/main.cpp)
target_link_libraries(draft-doctor PRIVATE miners_lib)

# Submodules 
add_subdirectory(tests)
add_subdirectory(benchmarks)
