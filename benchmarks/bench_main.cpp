#include <benchmark/benchmark.h>
#include "apriori.hpp"
#include "fpgrowth.hpp"
#include "utils.hpp"
#include <fstream>
#include <random>
#include <iostream>

// --- Helper: Generate a larger synthetic dataset ---
void generate_synthetic_data(const std::string& filename, int num_transactions) {
    std::ofstream out(filename);
    out << "gameid,team,ban1,ban2,ban3,ban4,ban5,pick1,pick2,pick3,pick4,pick5\n";

    // Simple random generator
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> champ_dist(1, 50); // 50 possible champions

    for (int i = 0; i < num_transactions; ++i) {
        out << "G" << i << ",Team1,N,N,N,N,N,"; // Ignore bans for speed
        // Pick 5 random champs
        for (int p = 0; p < 5; ++p) {
            out << "Champ" << champ_dist(rng);
            if (p < 4) out << ",";
        }
        out << "\n";
    }
    out.close();
}

// Global Loader to avoid reloading CSV for every test iteration
DataLoader* g_loader = nullptr;

// Setup step: Runs once before benchmarks start
void SetupData() {
    if (!g_loader) {
        generate_synthetic_data("bench_data.csv", 1000); // 1000 Games
        g_loader = new DataLoader();
        g_loader->load_data("bench_data.csv");
    }
}

// Cleanup step
void CleanupData() {
    if (g_loader) {
        delete g_loader;
        g_loader = nullptr;
        std::remove("bench_data.csv");
    }
}

// --- Benchmark: Apriori ---
static void BM_Apriori(benchmark::State& state) {
    if (!g_loader) SetupData();

    Apriori miner;
    // Run the loop
    for (auto _ : state) {
        // Min support = 5% of 1000 = 50
        auto results = miner.run(g_loader->get_transactions(), 50);
        benchmark::DoNotOptimize(results); // Prevent compiler form skipping code
    }
}
BENCHMARK(BM_Apriori)->Unit(benchmark::kMillisecond);

// --- Benchmark: FP-Growth ---
static void BM_FPGrowth(benchmark::State& state) {
    if (!g_loader) SetupData();

    FPGrowth miner;
    for (auto _ : state) {
        auto results = miner.run(g_loader->get_transactions(), 50);
        benchmark::DoNotOptimize(results);
    }
}
BENCHMARK(BM_FPGrowth)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();