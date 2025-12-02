#include <iostream>
#include <fstream>
#include "utils.hpp"
#include "apriori.hpp"
#include "fpgrowth.hpp"

void create_dummy_data(const std::string& filename) {
    std::ofstream out(filename);
    out << "gameid,team,ban1,ban2,ban3,ban4,ban5,pick1,pick2,pick3,pick4,pick5\n";
    out << "G1,T1,None,None,None,None,None,Ahri,Lee Sin,Vi,Jinx,Thresh\n";
    out << "G2,T1,None,None,None,None,None,Ahri,Lee Sin,Zed,Kai'sa,Nautilus\n";
    out << "G3,T1,None,None,None,None,None,Ahri,Renekton,Elise,Ashe,Braum\n";
    out.close();
}

void print_results(const std::string& algo_name, const std::vector<Pattern>& patterns, DataLoader& loader) {
    std::cout << "\n--- " << algo_name << " Results ---\n";
    for (const auto& p : patterns) {
        std::cout << "{ ";
        for (int id : p.items) std::cout << loader.get_champion_name(id) << " ";
        std::cout << "} : " << p.support << "\n";
    }
}

int main() {
    std::string test_file = "test_data.csv";
    create_dummy_data(test_file);

    DataLoader loader;
    if (loader.load_data(test_file)) {
        int min_sup = 2;

        // 1. Run Apriori
        Apriori apriori;
        auto results_apriori = apriori.run(loader.get_transactions(), min_sup);
        print_results("Apriori", results_apriori, loader);

        // 2. Run FP-Growth
        FPGrowth fpgrowth;
        auto results_fp = fpgrowth.run(loader.get_transactions(), min_sup);
        print_results("FP-Growth", results_fp, loader);
    }

    std::remove(test_file.c_str());
    return 0;
}
