#pragma once

#include "utils.hpp"
#include <vector>
#include <set>
#include <map>

// Represents a discovered frequent pattern 
struct Pattern {
    std::set<int> items; // Sorted set of item IDs
    int support;         // Absolute count of occurrences
};

class Apriori {
public:
    // Main Entry Point
    // min_sup_count: integer threshold (e.g., 100 games), not percentage.
    std::vector<Pattern> run(const std::vector<Transaction>& transactions, int min_sup_count);

private:
    // Internal State 
    int min_sup;
    std::vector<Pattern> frequent_itemsets; // Stores all results found so far

    // algo steps

    // step 1: Find individual frequent items (L1)
    // Returns a set of single-item sets that meet support.
    std::vector<std::set<int>> find_frequent_singles(const std::vector<Transaction>& transactions);

    // step 2: Generate Candidates (Ck) from Frequent Itemsets (Lk-1)
    // Joins two itemsets if they share the first k-2 items.
    std::vector<std::set<int>> generate_candidates(const std::vector<std::set<int>>& prev_frequent_sets, int k);

    // Step 3: Prune (this is optional but I think it's for the best?)
    // Removes candidates if any of their subsets are not in prev_frequent_sets.
    // (We can implement this optimization later, keeping the signature here).
    bool has_infrequent_subset(const std::set<int>& candidate, const std::set<std::set<int>>& prev_frequent_sets);

    // step 4: Count Support
    // Scans the database to count how many times each candidate appears.
    // Returns only those that meet min_sup.
    std::vector<std::set<int>> filter_candidates(const std::vector<Transaction>& transactions, 
                                                 const std::vector<std::set<int>>& candidates);
};
