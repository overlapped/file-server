# Benchmarks
find_package(benchmark REQUIRED)

add_executable(file_server_benchmarks
    BenchmarkServer.cpp
#    BenchmarkClient.cpp
)

target_link_libraries(file_server_benchmarks
    benchmark::benchmark
    benchmark::benchmark_main
    file_server_lib
    file_client_lib
    pthread
)
