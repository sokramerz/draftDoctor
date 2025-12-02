#include "fpgrowth.hpp"
#include <algorithm>
#include <iostream>

// Comparator for sorting items by frequency (Descending)
struct FrequencyComparator {
    const std::map<int, int>& counts;
    bool operator()(int a, int b) const {
        if (counts.at(a) != counts.at(b)) {
            return counts.at(a) > counts.at(b);
        }
        return a < b; // Tie-breaker
    }
};

std::vector<Pattern> FPGrowth::run(const std::vector<Transaction>& transactions, int min_sup_count) {
    this->min_sup = min_sup_count;
    this->frequent_patterns.clear();

    std::cout << "Running FP-Growth with MinSup: " << min_sup << "\n";

    // 1. Get Global Frequencies
    std::map<int, int> global_counts = get_frequent_counts(transactions);
    
    // 2. Build the initial Header Table and Tree
    std::map<int, std::shared_ptr<FPNode>> header_table;
    auto root = build_tree(transactions, global_counts, header_table);

    // 3. Mine recursively
    mine_tree(root, header_table, {});

    return frequent_patterns;
}

std::map<int, int> FPGrowth::get_frequent_counts(const std::vector<Transaction>& transactions) {
    std::map<int, int> counts;
    for (const auto& t : transactions) {
        for (int item : t.items) {
            counts[item]++;
        }
    }
    // Filter by min_sup immediately
    std::map<int, int> filtered;
    for (auto const& [item, count] : counts) {
        if (count >= min_sup) filtered[item] = count;
    }
    return filtered;
}

std::shared_ptr<FPNode> FPGrowth::build_tree(const std::vector<Transaction>& transactions, 
                                             const std::map<int, int>& global_freq_items,
                                             std::map<int, std::shared_ptr<FPNode>>& header_table) {
    
    auto root = std::make_shared<FPNode>(-1, nullptr); // -1 is root

    for (const auto& t : transactions) {
        // Filter and Sort transaction items
        std::vector<int> sorted_items;
        for (int item : t.items) {
            if (global_freq_items.find(item) != global_freq_items.end()) {
                sorted_items.push_back(item);
            }
        }
        
        // Sort by global frequency (Descending)
        std::sort(sorted_items.begin(), sorted_items.end(), FrequencyComparator{global_freq_items});

        // Insert into Tree
        std::shared_ptr<FPNode> current = root;
        for (int item : sorted_items) {
            std::shared_ptr<FPNode> child = nullptr;
            for (auto& c : current->children) {
                if (c->item_id == item) {
                    child = c;
                    break;
                }
            }

            if (child) {
                child->count++;
            } else {
                child = std::make_shared<FPNode>(item, current);
                current->children.push_back(child);

                // Update Header Table (Linked List)
                if (header_table.find(item) == header_table.end()) {
                    header_table[item] = child;
                } else {
                    // Prepend or Append? Usually append, but prepend is O(1) if we don't care about order.
                    // Standard FP-Growth uses the header to traverse ALL nodes of that item.
                    // We need to insert this 'child' into the list starting at header_table[item].
                    child->next_link = header_table[item];
                    header_table[item] = child;
                }
            }
            current = child;
        }
    }
    return root;
}

void FPGrowth::mine_tree(std::shared_ptr<FPNode> tree_root, 
                         std::map<int, std::shared_ptr<FPNode>>& header_table, 
                         std::set<int> current_suffix) {
    
    // Iterate over items in header table (simplest: iterate map)
    // Theoretically should iterate in increasing order of frequency
    for (auto const& [item, node_chain_head] : header_table) {
        
        // --Calculate Support for this item (sum of counts in its chain)
        int support = 0;
        std::shared_ptr<FPNode> curr = node_chain_head;
        while (curr) {
            support += curr->count;
            curr = curr->next_link;
        }

        if (support < min_sup) continue;

        // --Add to results
        std::set<int> new_pattern = current_suffix;
        new_pattern.insert(item);
        frequent_patterns.push_back({new_pattern, support});

        // --Build Conditional Pattern Base
        // For every node in the chain, walk UP to root to find the path.
        // The path (prefix) happens 'node.count' times.
        std::vector<Transaction> conditional_transactions;
        
        curr = node_chain_head;
        while (curr) {
            std::vector<int> path;
            std::shared_ptr<FPNode> parent = curr->parent.lock(); // Use lock() for weak_ptr
            
            while (parent && parent->item_id != -1) { // -1 is root
                path.push_back(parent->item_id);
                parent = parent->parent.lock();
            }

            if (!path.empty()) {
                // The path happened 'curr->count' times. 
                // We add it to the conditional DB that many times (or optimize by using weights).
                // For simplicity here, we duplicate the transaction or modify build_tree to accept weights.
                // Simpler Approach for this project: Add it once, but we need our Tree Builder to handle weights.
                // *Refactoring*: Let's just add it 'curr->count' times to be safe and simple for now.
                Transaction t;
                t.items = path; 
                // We reverse because we walked up, but order doesn't strictly matter for the set, 
                // but build_tree re-sorts anyway.
                for(int k=0; k < curr->count; k++) {
                   conditional_transactions.push_back(t);
                }
            }
            curr = curr->next_link;
        }

        // Build Conditional Tree
        if (!conditional_transactions.empty()) {
            std::map<int, int> cond_counts = get_frequent_counts(conditional_transactions);
            std::map<int, std::shared_ptr<FPNode>> cond_header;
            auto cond_root = build_tree(conditional_transactions, cond_counts, cond_header);
            
            if (!cond_header.empty()) {
                mine_tree(cond_root, cond_header, new_pattern);
            }
        }
    }
}
