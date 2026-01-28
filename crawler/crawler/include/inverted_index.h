#include <string>
#include <vector>
#include <memory>
#include <leveldb/db.h>

/**
 * @class InvertedIndex
 * @brief High-performance, LevelDB-backed inverted index.
 */
class InvertedIndex {
public:
    InvertedIndex(const std::string& db_path);
    void add_document(const std::string& doc_id, const std::vector<std::string>& tokens);
    std::vector<std::string> lookup(const std::string& token) const;
    void remove_document(const std::string& doc_id);
    // Incremental update support
    void apply_diff(const std::vector<std::string>& updated_tokens);
    // Expose db_ for search_service.cpp (not best practice, but for demo)
    std::unique_ptr<leveldb::DB>& get_db() { return db_; }

private:
    std::unique_ptr<leveldb::DB> db_;
}; 