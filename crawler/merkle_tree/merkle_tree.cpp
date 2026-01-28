#include "merkle_tree.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <algorithm>

/**
 * @brief Helper function to hash two strings (hex-encoded SHA-256) into a parent node.
 */
std::string MerkleTree::hash_pair(const std::string& left, const std::string& right) {
    std::string combined = left + right;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, combined.c_str(), combined.size());
    SHA256_Final(hash, &sha256);
    std::ostringstream oss;
    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return oss.str();
}

/**
 * @brief Build the Merkle tree from the leaves, populating tree_levels_.
 *        Each level is a vector of hashes, with the root at the last level.
 */
void MerkleTree::build_tree() {
    tree_levels_.clear();
    if (leaves_.empty()) return;
    tree_levels_.push_back(leaves_);
    std::vector<std::string> current = leaves_;
    while (current.size() > 1) {
        std::vector<std::string> next_level;
        for (size_t i = 0; i < current.size(); i += 2) {
            if (i + 1 < current.size()) {
                // Hash pair of children
                next_level.push_back(hash_pair(current[i], current[i + 1]));
            } else {
                // Odd node: duplicate last
                next_level.push_back(hash_pair(current[i], current[i]));
            }
        }
        tree_levels_.push_back(next_level);
        current = next_level;
    }
}

/**
 * @brief Construct a Merkle tree from a sequence of block hashes (leaves).
 *        Automatically builds the tree structure.
 */
MerkleTree::MerkleTree(const std::vector<std::string>& block_hashes)
    : leaves_(block_hashes) {
    build_tree();
}

/**
 * @brief Get the Merkle root hash (hex-encoded SHA-256).
 *        Returns empty string if tree is empty.
 */
std::string MerkleTree::root_hash() const {
    if (tree_levels_.empty()) return "";
    return tree_levels_.back().front();
}

/**
 * @brief Compute the list of updated hashes (leaves) compared to another tree.
 *        Returns the hashes of leaves that differ between the two trees.
 *        (Assumes both trees have the same number/order of leaves.)
 */
std::vector<std::string> MerkleTree::diff(const MerkleTree& other) const {
    std::vector<std::string> diffs;
    size_t n = std::min(leaves_.size(), other.leaves_.size());
    for (size_t i = 0; i < n; ++i) {
        if (leaves_[i] != other.leaves_[i]) {
            diffs.push_back(leaves_[i]);
        }
    }
    // If this tree has extra leaves, consider them as diffs
    for (size_t i = n; i < leaves_.size(); ++i) {
        diffs.push_back(leaves_[i]);
    }
    return diffs;
} 