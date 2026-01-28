#include "content_store.h"
#include <leveldb/db.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <stdexcept>

/**
 * @brief Helper function to convert a byte array to a hex string.
 */
static std::string bytes_to_hex(const unsigned char* bytes, size_t len) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)bytes[i];
    }
    return oss.str();
}

/**
 * @brief Construct a ContentStore instance and open the LevelDB database.
 *        Throws std::runtime_error if the database cannot be opened.
 */
ContentStore::ContentStore(const std::string& db_path) {
    leveldb::DB* db = nullptr;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, db_path, &db);
    if (!status.ok()) {
        throw std::runtime_error("Failed to open LevelDB: " + status.ToString());
    }
    db_.reset(db);
}

/**
 * @brief Destructor. Closes the LevelDB database.
 */
ContentStore::~ContentStore() {
    // db_ is a unique_ptr, so it will be cleaned up automatically.
}

/**
 * @brief Store a block of data, returning its SHA-256 hash.
 *        If the block already exists, it is not duplicated.
 */
std::string ContentStore::store_block(const std::string& data) {
    std::string hash = sha256(data);
    // Only store if not already present
    std::string existing;
    leveldb::Status s = db_->Get(leveldb::ReadOptions(), hash, &existing);
    if (!s.ok()) {
        db_->Put(leveldb::WriteOptions(), hash, data);
    }
    return hash;
}

/**
 * @brief Retrieve a block by its hash.
 */
std::string ContentStore::get_block(const std::string& hash) const {
    std::string value;
    leveldb::Status s = db_->Get(leveldb::ReadOptions(), hash, &value);
    if (s.ok()) {
        return value;
    }
    return ""; // Not found
}

/**
 * @brief Chunk input data into 4KB (or smaller) blocks.
 *        This enables deduplication and efficient storage.
 */
std::vector<std::string> ContentStore::chunk_data(const std::string& data, size_t chunk_size) {
    std::vector<std::string> blocks;
    size_t total = data.size();
    for (size_t i = 0; i < total; i += chunk_size) {
        blocks.push_back(data.substr(i, std::min(chunk_size, total - i)));
    }
    return blocks;
}

/**
 * @brief Store multiple blocks, returning their hashes.
 *        Useful for storing a whole document efficiently.
 */
std::vector<std::string> ContentStore::store_blocks(const std::vector<std::string>& blocks) {
    std::vector<std::string> hashes;
    for (const auto& block : blocks) {
        hashes.push_back(store_block(block));
    }
    return hashes;
}

/**
 * @brief Retrieve multiple blocks by their hashes.
 */
std::vector<std::string> ContentStore::get_blocks(const std::vector<std::string>& hashes) const {
    std::vector<std::string> blocks;
    for (const auto& hash : hashes) {
        blocks.push_back(get_block(hash));
    }
    return blocks;
}

/**
 * @brief Compute SHA-256 hash of a data block using OpenSSL.
 *        Returns the hash as a hex-encoded string.
 */
std::string ContentStore::sha256(const std::string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(hash, &sha256);
    return bytes_to_hex(hash, SHA256_DIGEST_LENGTH);
} 