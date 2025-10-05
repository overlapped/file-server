#include <benchmark/benchmark.h>
#include "server/DynamicFileGenerator.h"

static void BM_ChunkGeneration(benchmark::State& state) {
    DynamicFileGenerator generator;
    size_t chunk_size = state.range(0);
    
    for (auto _ : state) {
        auto chunk = generator.generateChunk(1, state.iterations(), chunk_size);
        benchmark::DoNotOptimize(chunk);
    }
    
    state.SetBytesProcessed(state.iterations() * chunk_size);
}

BENCHMARK(BM_ChunkGeneration)
    ->Arg(1024)        // 1KB
    ->Arg(1024*1024)   // 1MB
    ->Arg(10*1024*1024); // 10MB

BENCHMARK_MAIN();
