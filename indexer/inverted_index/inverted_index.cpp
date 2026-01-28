#include "inverted_index.h"
#include <leveldb/db.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <stdexcept>

/**
 * @brief Construct an InvertedIndex backed by LevelDB.
 */
InvertedIndex::InvertedIndex(const std::string& db_path) {
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::DB* db = nullptr;
    leveldb::Status status = leveldb::DB::Open(options, db_path, &db);
    if (!status.ok()) {
        throw std::runtime_error("Failed to open LevelDB: " + status.ToString());
    }
    db_.reset(db);
}

/**
 * @brief Add a document to the inverted index.
 *        Each token is mapped to the doc_id (posting list).
 */
void InvertedIndex::add_document(const std::string& doc_id, const std::vector<std::string>& tokens) {
    std::unordered_set<std::string> unique_tokens(tokens.begin(), tokens.end());
    for (const auto& token : unique_tokens) {
        std::string posting_list;
        db_->Get(leveldb::ReadOptions(), token, &posting_list);
        // Append doc_id if not already present
        if (posting_list.find(doc_id) == std::string::npos) {
            if (!posting_list.empty()) posting_list += ",";
            posting_list += doc_id;
            db_->Put(leveldb::WriteOptions(), token, posting_list);
        }
    }
}

/**
 * @brief Lookup the posting list (doc IDs) for a token.
 */
std::vector<std::string> InvertedIndex::lookup(const std::string& token) const {
    std::string posting_list;
    std::vector<std::string> doc_ids;
    if (db_->Get(leveldb::ReadOptions(), token, &posting_list).ok()) {
        std::istringstream iss(posting_list);
        std::string doc_id;
        while (std::getline(iss, doc_id, ',')) {
            if (!doc_id.empty()) doc_ids.push_back(doc_id);
        }
    }
    return doc_ids;
}

/**
 * @brief Remove a document from the inverted index.
 */
void InvertedIndex::remove_document(const std::string& doc_id) {
    // Inefficient: scan all tokens (for demo; optimize with reverse index in production)
    std::unique_ptr<leveldb::Iterator> it(db_->NewIterator(leveldb::ReadOptions()));
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string token = it->key().ToString();
        std::string posting_list = it->value().ToString();
        std::vector<std::string> doc_ids;
        std::istringstream iss(posting_list);
        std::string id;
        while (std::getline(iss, id, ',')) {
            if (!id.empty() && id != doc_id) doc_ids.push_back(id);
        }
        // Rebuild posting list
        std::ostringstream oss;
        for (size_t i = 0; i < doc_ids.size(); ++i) {
            if (i > 0) oss << ",";
            oss << doc_ids[i];
        }
        db_->Put(leveldb::WriteOptions(), token, oss.str());
    }
}

/**
 * @brief Apply incremental updates to the index (placeholder for Merkle diff logic).
 */
void InvertedIndex::apply_diff(const std::vector<std::string>& updated_tokens) {
    // TODO: Implement efficient incremental update using Merkle diffs
} 