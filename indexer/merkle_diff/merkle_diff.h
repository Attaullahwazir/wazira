#include <vector>
#include <string>

/**
 * @class MerkleDiff
 * @brief Computes and applies Merkle diffs for index updates.
 */
class MerkleDiff {
public:
    static std::vector<std::string> compute_diff(const std::vector<std::string>& old_tokens, const std::vector<std::string>& new_tokens);
}; 