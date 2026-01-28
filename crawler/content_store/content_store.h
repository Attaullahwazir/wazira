#ifndef CONTENT_STORE_H
#define CONTENT_STORE_H

#include <string>
#include <vector>
#include <memory>

// Forward declaration for LevelDB
namespace leveldb {
    class DB;
}

/**
 * @class ContentStore
 * @brief Content-addressed storage for chunked web content using LevelDB and SHA-256.
 *
 * Chunks input data into 4KB blocks, hashes each block, and stores them in LevelDB.
 * Provides methods to store and retrieve blocks by their hash.
 */
class ContentStore {
public:
    /**
     * @brief Construct a ContentStore instance.
     * @param db_path Path to the LevelDB database directory.
     */
    explicit ContentStore(const std::string& db_path);

    /**
     * @brief Destructor. Closes the LevelDB database.
     */
    ~ContentStore();

    /**
     * @brief Store a block of data, returning its SHA-256 hash.
     * @param data The data block to store (should be <= 4KB).
     * @return The SHA-256 hash of the block (hex-encoded).
     */
    std::string store_block(const std::string& data);

    /**
     * @brief Retrieve a block by its hash.
     * @param hash The SHA-256 hash of the block (hex-encoded).
     * @return The data block, or empty string if not found.
     */
    std::string get_block(const std::string& hash) const;

    /**
     * @brief Chunk input data into 4KB blocks.
     * @param data The input data to chunk.
     * @return Vector of 4KB (or smaller) blocks.
     */
    static std::vector<std::string> chunk_data(const std::string& data, size_t chunk_size = 4096);

    /**
     * @brief Store multiple blocks, returning their hashes.
     * @param blocks Vector of data blocks.
     * @return Vector of SHA-256 hashes (hex-encoded).
     */
    std::vector<std::string> store_blocks(const std::vector<std::string>& blocks);

    /**
     * @brief Retrieve multiple blocks by their hashes.
     * @param hashes Vector of SHA-256 hashes (hex-encoded).
     * @return Vector of data blocks (empty string if not found).
     */
    std::vector<std::string> get_blocks(const std::vector<std::string>& hashes) const;

    /**
     * @brief Compute SHA-256 hash of a data block.
     * @param data The data to hash.
     * @return The SHA-256 hash (hex-encoded).
     */
    static std::string sha256(const std::string& data);

private:
    std::unique_ptr<leveldb::DB> db_;
};

#endif // CONTENT_STORE_H 