// merkle_tree.h
// Merkle tree for efficient diff computation of content blocks
// Part of the crawler module for the next-gen search engine
//
// Responsibilities:
// - Constructs a Merkle tree from a sequence of block hashes
// - Computes root hash
// - Computes diffs between trees (list of updated hashes)
//
// Planned interface:
//   class MerkleTree {
//     public:
//       MerkleTree(const std::vector<std::string>& block_hashes);
//       std::string root_hash() const;
//       std::vector<std::string> diff(const MerkleTree& other) const;
//   };

#ifndef MERKLE_TREE_H
#define MERKLE_TREE_H

#include <string>
#include <vector>

/**
 * @class MerkleTree
 * @brief Binary Merkle tree for content block hashes.
 *
 * Constructs a binary Merkle tree from a vector of leaf hashes (block hashes).
 * Supports efficient computation of the root hash and diffs between trees.
 */
class MerkleTree {
public:
    /**
     * @brief Construct a Merkle tree from a sequence of block hashes (leaves).
     * @param block_hashes Vector of leaf hashes (hex-encoded SHA-256).
     */
    MerkleTree(const std::vector<std::string>& block_hashes);

    /**
     * @brief Get the Merkle root hash (hex-encoded SHA-256).
     * @return The root hash of the tree.
     */
    std::string root_hash() const;

    /**
     * @brief Compute the list of updated hashes (leaves) compared to another tree.
     * @param other The other MerkleTree to compare against.
     * @return Vector of hashes that differ between the two trees.
     */
    std::vector<std::string> diff(const MerkleTree& other) const;

private:
    std::vector<std::string> leaves_; ///< Leaf hashes (block hashes)
    std::vector<std::vector<std::string>> tree_levels_; ///< Each level of the tree, bottom-up

    /**
     * @brief Build the tree from the leaves, populating tree_levels_.
     */
    void build_tree();

    /**
     * @brief Compute the hash of two child nodes (internal node hash).
     * @param left Left child hash.
     * @param right Right child hash.
     * @return Parent node hash (hex-encoded SHA-256).
     */
    static std::string hash_pair(const std::string& left, const std::string& right);
};

#endif // MERKLE_TREE_H 