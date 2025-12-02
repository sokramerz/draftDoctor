#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>

std::string clean_cell(const std::string& input) {
    if (input.size() >= 2 && input.front() == '"' && input.back() == '"') {
        return input.substr(1, input.size() - 2);
    }
    return input;
}

int DataLoader::get_champion_id(const std::string& name) {
    if (name.empty()) return 0;
    if (name_to_id.find(name) == name_to_id.end()) {
        name_to_id[name] = next_id;
        id_to_name[next_id] = name;
        next_id++;
    }
    return name_to_id[name];
}

std::string DataLoader::get_champion_name(int id) const {
    if (id_to_name.count(id)) return id_to_name.at(id);
    return "Unknown";
}

const std::vector<Transaction>& DataLoader::get_transactions() const {
    return transactions;
}

bool DataLoader::load_data(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << file_path << "\n";
        return false;
    }

    std::string line;
    std::vector<int> target_indices;
    
    // Read Header
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string header;
        int idx = 0;
        while (std::getline(ss, header, ',')) {
            header = clean_cell(header);
            // Detect Oracle's Elixir columns (pick1..pick5, ban1..ban5)
            if (header.find("ban") != std::string::npos || header.find("pick") != std::string::npos) {
                target_indices.push_back(idx);
            }
            idx++;
        }
    }

    if (target_indices.empty()) {
        std::cerr << "Error: No 'pick' or 'ban' columns found in header.\n";
        return false;
    }

    // Read Rows
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row_data;
        
        // Basic CSV split
        while (std::getline(ss, cell, ',')) {
            row_data.push_back(clean_cell(cell));
        }

        std::set<int> unique_items;
        for (int idx : target_indices) {
            if (idx < row_data.size()) {
                std::string val = row_data[idx];
                if (!val.empty() && val != "None") {
                    unique_items.insert(get_champion_id(val));
                }
            }
        }

        if (!unique_items.empty()) {
            Transaction t;
            t.items.assign(unique_items.begin(), unique_items.end());
            transactions.push_back(t);
        }
    }
    return true;
}

void DataLoader::print_stats() const {
    std::cout << "Dataset Stats:\n";
    std::cout << "  Transactions: " << transactions.size() << "\n";
    std::cout << "  Unique Champions: " << name_to_id.size() << "\n";
}
