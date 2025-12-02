#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

// Helper to strip quotes if present
std::string clean_cell(const std::string& input) {
    if (input.size() >= 2 && input.front() == '"' && input.back() == '"') {
        return input.substr(1, input.size() - 2);
    }
    return input;
}

int DataLoader::get_champion_id(const std::string& name) {
    if (name.empty()) return 0; // Skip empty cells, standard 
    
    if (name_to_id.find(name) == name_to_id.end()) {
        name_to_id[name] = next_id;
        id_to_name[next_id] = name;
        next_id++;
    }
    return name_to_id[name];
}

std::string DataLoader::get_champion_name(int id) const {
    auto it = id_to_name.find(id);
    if (it != id_to_name.end()) {
        return it->second;
    }
    return "Unknown";
}

const std::vector<Transaction>& DataLoader::get_transactions() const {
    return transactions;
}

bool DataLoader::load_data(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << file_path << "\n";
        return false;
    }

    std::string line;
    std::vector<std::string> headers;
    
    // Read Header
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            headers.push_back(clean_cell(cell));
        }
    }

    // Identify Column Indices for Bans and Picks
    std::vector<int> target_indices;
    for (size_t i = 0; i < headers.size(); ++i) {
        std::string h = headers[i];
        // Oracle's Elixir uses "ban1"..."ban5" and "pick1"..."pick5"
        // we look for these substrings.
        if (h.find("ban") != std::string::npos || h.find("pick") != std::string::npos) {
            target_indices.push_back(i);
        }
    }

    if (target_indices.empty()) {
        std::cerr << "Warning: No 'ban' or 'pick' columns found in CSV header.\n";
        return false;
    }

    // Read Rows
    int rows_processed = 0;
    while (std::getline(file, line)) {
        // Simple CSV split (handling standard comma separation)
        std::vector<std::string> row_data;
        std::stringstream ss(line);
        std::string cell;
        
        // Note: This simple split breaks on commas inside quotes, but Champion names don't have commas.
        while (std::getline(ss, cell, ',')) {
            row_data.push_back(clean_cell(cell));
        }

        // Build Transaction
        std::set<int> unique_items; // use set to auto-sort and dedup
        for (int idx : target_indices) {
            if (idx < row_data.size()) {
                std::string champ = row_data[idx];
                if (!champ.empty() && champ != "None") { // FIXED: Handle missing bans
                    unique_items.insert(get_champion_id(champ));
                }
            }
        }

        if (!unique_items.empty()) {
            Transaction t;
            t.items.assign(unique_items.begin(), unique_items.end());
            transactions.push_back(t);
        }
        rows_processed++;
    }

    std::cout << "Successfully loaded " << rows_processed << " rows.\n";
    return true;
}

void DataLoader::print_stats() const {
    std::cout << "Dataset Stats:\n";
    std::cout << "  - Transactions: " << transactions.size() << "\n";
    std::cout << "  - Unique Champions: " << name_to_id.size() << "\n";
}
