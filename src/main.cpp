#include <iostream>
#include <string>
#include <vector>
#include "utils.hpp"
#include "fpgrowth.hpp"

int main(int argc, char* argv[]) {
    // Get File Path from Command Line
    std::string filename = "";
    if (argc > 1) {
        filename = argv[1];
    } else {
        std::cout << "Usage: draft-doctor.exe <csv_file_path>\n";
        std::cout << "Error: No file provided.\n";
        return 1;
    }

    // Load Data
    std::cout << "--- Draft Doctor: Mining " << filename << " ---\n";
    
    DataLoader loader;
    // We print a loading message because large files take 1-2 seconds
    std::cout << "Loading transactions... "; 
    if (!loader.load_data(filename)) {
        std::cout << "FAILED.\n";
        return 1;
    }
    std::cout << "DONE.\n";
    loader.print_stats();

    // Set Support Threshold 
    // In a 5000 game dataset, 1% = 50 games.
    double support_percent = 0.01; 
    int min_sup = (int)(loader.get_transactions().size() * support_percent);
    if (min_sup < 5) min_sup = 5; // Safety net

    std::cout << "Mining with Minimum Support: " << min_sup << " (" << (support_percent*100) << "%)\n";

    // Run FP-Growth
    FPGrowth miner;
    auto patterns = miner.run(loader.get_transactions(), min_sup);

    // Show Top Synergies (Pairs only)
    std::cout << "\n--- Top Frequent Synergies ---\n";
    int count = 0;
    for (const auto& p : patterns) {
        // We only care about pairs (size 2) or more
        if (p.items.size() >= 2) { 
            std::cout << "[ ";
            for (int id : p.items) std::cout << loader.get_champion_name(id) << " ";
            std::cout << "] : " << p.support << " games\n";
            count++;
        }
        if (count >= 25) break; // Show top 25
    }
    
    return 0;
}
