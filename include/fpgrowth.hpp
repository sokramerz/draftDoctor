#pragma once

#include "utils.hpp"
#include "apriori.hpp" // We re-use the Pattern struct
#include <vector>
#include <map>
#include <memory>
#include <set>

// A node in the FP-Tree
struct FPNode {
    int item_id;
    int count;
    std::weak_ptr<FPNode> parent; // Weak pointer to avoid cycles
    std::vector<std::shared_ptr<FPNode>> children;
    std::shared_ptr<FPNode> next_link; // For the header table (linked list of same items)

    FPNode(int item, std::shared_ptr<FPNode> p) : item_id(item), count(1), parent(p), next_link(nullptr) {}
};

class FPGrowth {
public:
    // Main Entry Point
    std::vector<Pattern> run(const std::vector<Transaction>& transactions, int min_sup_count);

private:
    int min_sup;
    std::vector<Pattern> frequent_patterns;

    // helperss
    
    // Build the main FP-Tree from transactions
    std::shared_ptr<FPNode> build_tree(const std::vector<Transaction>& transactions, 
                                       const std::map<int, int>& global_freq_items,
                                       std::map<int, std::shared_ptr<FPNode>>& header_table);

    // Recursive Miner (The "Growth" phase)
    // mines the tree rooted at 'tree_root' for specific conditional patterns
    void mine_tree(std::shared_ptr<FPNode> tree_root, 
                   std::map<int, std::shared_ptr<FPNode>>& header_table, 
                   std::set<int> current_suffix);

    // Find global frequency of single items (L1) - reuse logic effectively
    std::map<int, int> get_frequent_counts(const std::vector<Transaction>& transactions);
};
