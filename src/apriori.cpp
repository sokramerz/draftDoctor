#include "apriori.hpp"
#include <map>
#include <algorithm>
#include <iostream>

std::vector<Pattern> Apriori::run(const std::vector<Transaction>& transactions, int min_sup_count) {
    this->min_sup = min_sup_count;
    frequent_itemsets.clear();

    // Step 1: Find L1
    auto L = find_frequent_singles(transactions);
    int k = 2;

    // Loop until no more frequent itemsets
    while (!L.empty()) {
        // Save current level to results
        for (const auto& itemset : L) {
            int count = 0;
            for (const auto& t : transactions) {
                if (std::includes(t.items.begin(), t.items.end(), itemset.begin(), itemset.end())) {
                    count++;
                }
            }
            frequent_itemsets.push_back({ itemset, count });
        }

        // Generate Candidates
        auto C = generate_candidates(L, k);

        // Filter Candidates
        if (C.empty()) break;
        L = filter_candidates(transactions, C);
        k++;
    }
    return frequent_itemsets;
}

std::vector<std::set<int>> Apriori::find_frequent_singles(const std::vector<Transaction>& transactions) {
    std::map<int, int> counts;
    for (const auto& t : transactions) {
        for (int i : t.items) {
            counts[i]++;
        }
    }

    std::vector<std::set<int>> res;
    for (auto const& [id, c] : counts) {
        if (c >= min_sup) {
            res.push_back({ id });
        }
    }
    return res;
}

std::vector<std::set<int>> Apriori::generate_candidates(const std::vector<std::set<int>>& prev, int k) {
    std::vector<std::set<int>> cands;
    size_t n = prev.size(); // Changed to size_t to fix warning

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            std::vector<int> v1(prev[i].begin(), prev[i].end());
            std::vector<int> v2(prev[j].begin(), prev[j].end());

            bool match = true;
            // Check if first k-2 items match
            for (int x = 0; x < k - 2; ++x) {
                if (v1[x] != v2[x]) {
                    match = false;
                    break;
                }
            }

            if (match) {
                std::set<int> new_set = prev[i];
                new_set.insert(v2.back());
                cands.push_back(new_set);
            }
        }
    }
    return cands;
}

std::vector<std::set<int>> Apriori::filter_candidates(const std::vector<Transaction>& trans, const std::vector<std::set<int>>& cands) {
    std::map<std::vector<int>, int> counts;

    for (const auto& t : trans) {
        for (const auto& c : cands) {
            if (std::includes(t.items.begin(), t.items.end(), c.begin(), c.end())) {
                std::vector<int> key(c.begin(), c.end());
                counts[key]++;
            }
        }
    }

    std::vector<std::set<int>> res;
    for (auto const& [key, c] : counts) {
        if (c >= min_sup) {
            res.push_back({ key.begin(), key.end() });
        }
    }
    return res;
}