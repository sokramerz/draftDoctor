#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

// A "Transaction" represents one team's set of picks and bans in a single game.
// We use integers (IDs) instead of strings for performance during mining.
struct Transaction {
    std::vector<int> items; // Sorted list of Champion IDs
};

class DataLoader {
public:
    // Load CSV from file_path.
    // Expected format: Oracle's Elixir (Team rows), looking for specific pick/ban columns.
    bool load_data(const std::string& file_path);

    // Get the list of all transactions processed
    const std::vector<Transaction>& get_transactions() const;

    // Mapping helpers
    int get_champion_id(const std::string& name);
    std::string get_champion_name(int id) const;
    
    // Debug helper
    void print_stats() const;

private:
    std::vector<Transaction> transactions;
    
    // Bi-directional mapping: Name <-> ID
    std::map<std::string, int> name_to_id;
    std::map<int, std::string> id_to_name;
    int next_id = 1; // Start IDs at 1
};
