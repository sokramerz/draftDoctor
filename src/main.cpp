#include <iostream>
#include <string>
#include <vector>
#include <chrono> 
#include <algorithm> // ADDED FOR SORTING
#include "utils.hpp"
#include "apriori.hpp"
#include "fpgrowth.hpp"

void print_header() {
    std::cout << "\n-------------------------------------------\n";
    std::cout << "      DRAFT DOCTOR: HEXTECH MINER v1.0      \n";
    std::cout << "---------------------------------------------\n";
}

int get_threshold(const DataLoader& loader) {
    std::cout << "\nEnter Minimum Support % (e.g., 1 for 1%): ";
    double percent;
    std::cin >> percent;
    int min_sup = (int)(loader.get_transactions().size() * (percent / 100.0));
    if (min_sup < 1) min_sup = 1;
    return min_sup;
}

// FIXED: Now sorts by Support before printing
void print_top_patterns(const std::vector<Pattern>& patterns, const DataLoader& loader) {
    // KINDA FIXED: Make a copy so we can sort it *need to revisit
    std::vector<Pattern> sorted = patterns;
    
    // Sort Descending (Highest Support First)
    std::sort(sorted.begin(), sorted.end(), [](const Pattern& a, const Pattern& b) {
        return a.support > b.support;
    });

    std::cout << "\n--- Top Synergies (Highest Support) ---\n";
    int count = 0;
    for (const auto& p : sorted) {
        if (p.items.size() >= 2) {
            std::cout << "[ ";
            for (int id : p.items) std::cout << loader.get_champion_name(id) << " ";
            std::cout << "] : " << p.support << "\n";
            count++;
        }
        if (count >= 10) break;
    }
}

void run_apriori(DataLoader& loader) {
    if (loader.get_transactions().empty()) return;
    int min_sup = get_threshold(loader);
    std::cout << "Mining with Apriori (Support: " << min_sup << ")... Please wait.\n";

    auto start = std::chrono::high_resolution_clock::now();
    Apriori miner;
    auto patterns = miner.run(loader.get_transactions(), min_sup);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Done! Found " << patterns.size() << " patterns in " << elapsed.count() << " ms.\n";
    
    print_top_patterns(patterns, loader);
}

void run_fpgrowth(DataLoader& loader) {
    if (loader.get_transactions().empty()) return;
    int min_sup = get_threshold(loader);
    std::cout << "Mining with FP-Growth (Support: " << min_sup << ")... \n";

    auto start = std::chrono::high_resolution_clock::now();
    FPGrowth miner;
    auto patterns = miner.run(loader.get_transactions(), min_sup);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Done! Found " << patterns.size() << " patterns in " << elapsed.count() << " ms.\n";
    
    print_top_patterns(patterns, loader);
}

void run_race(DataLoader& loader) {
    if (loader.get_transactions().empty()) return;
    std::cout << "\n--- ALGORITHM RACE MODE ---\n";
    int min_sup = get_threshold(loader);

    //  apriori
    std::cout << "1. Running Apriori... ";
    auto start1 = std::chrono::high_resolution_clock::now();
    Apriori ap;
    auto p1 = ap.run(loader.get_transactions(), min_sup);
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> t1 = end1 - start1;
    std::cout << t1.count() << " ms.\n";

    // FP-Growth
    std::cout << "2. Running FP-Growth... ";
    auto start2 = std::chrono::high_resolution_clock::now();
    FPGrowth fp;
    auto p2 = fp.run(loader.get_transactions(), min_sup);
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> t2 = end2 - start2;
    std::cout << t2.count() << " ms.\n";

    // Result
    if (t2.count() > 0.0) {
        std::cout << "\n>>> Speedup: FP-Growth was " << (t1.count() / t2.count()) << "x faster.\n";
    } else {
        std::cout << "\n>>> Speedup: FP-Growth finished too quickly to measure reliably.\n";
    }

    // Both will now print identical top lists
    print_top_patterns(p2, loader);
}

int main(int argc, char* argv[]) {
    DataLoader loader;
    std::string filename;
    print_header();

    if (argc > 1) {
        filename = argv[1];
        std::cout << "Loading: " << filename << "... ";
        if (loader.load_data(filename)) {
            std::cout << "DONE.\n";
            loader.print_stats();
        } else {
            std::cout << "FAILED.\n";
        }
    }

    int choice = 0;
    while (choice != 5) {
        std::cout << "\n--- MAIN MENU ---\n";
        std::cout << "1. Load Data File\n";
        std::cout << "2. Run Apriori Mining\n";
        std::cout << "3. Run FP-Growth Mining\n";
        std::cout << "4. RACE: Apriori vs FP-Growth\n";
        std::cout << "5. Exit\n";
        std::cout << "Select: ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear(); 
            std::cin.ignore(10000, '\n'); 
            continue;
        }

        switch (choice) {
            case 1:
                std::cout << "Path: "; std::cin >> filename;
                if (filename.front() == '"') filename = filename.substr(1, filename.size()-2);
                if (loader.load_data(filename)) {
                    std::cout << "Loaded successfully.\n";
                    loader.print_stats(); 
                } else {
                    std::cout << "Failed to load file.\n";
                }
                break;
            case 2: run_apriori(loader); break;
            case 3: run_fpgrowth(loader); break;
            case 4: run_race(loader); break;
            case 5: break;
            default: std::cout << "Invalid.\n";
        }
    }
    return 0;
}
