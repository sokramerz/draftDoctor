add_executable(bench_run bench_main.cpp)
target_link_libraries(bench_run PRIVATE miners_lib benchmark::benchmark_main)
