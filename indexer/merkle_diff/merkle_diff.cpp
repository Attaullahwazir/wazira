#include "merkle_diff.h"
#include <vector>
#include <string>
#include <unordered_set>

/**
 * @brief Compute the diff between two token sets (new vs old).
 *        Returns tokens present in new_tokens but not in old_tokens.
 */
std::vector<std::string> MerkleDiff::compute_diff(const std::vector<std::string>& old_tokens, const std::vector<std::string>& new_tokens) {
    std::unordered_set<std::string> old_set(old_tokens.begin(), old_tokens.end());
    std::vector<std::string> diff;
    for (const auto& token : new_tokens) {
        if (old_set.find(token) == old_set.end()) {
            diff.push_back(token);
        }
    }
    return diff;
    // TODO: Optimize for large sets, support deletions, and Merkle tree-based diffs
} 