#include "inverted_index/inverted_index.h"
#include "include/httplib.h"
#include <leveldb/db.h>
#include <nlohmann/json.hpp> // For JSON serialization (https://github.com/nlohmann/json)
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>

using json = nlohmann::json;

// Simple BM25 parameters
constexpr double k1 = 1.5;
constexpr double b = 0.75;

// Placeholder: In production, store and update these stats efficiently
std::unordered_map<std::string, int> doc_lengths;
int total_docs = 0;
double avg_doc_length = 0.0;

// Compute BM25 score for a document given query tokens
// (Assumes doc_lengths and total_docs are up-to-date)
double bm25_score(const std::vector<std::string>& query_tokens, const std::string& doc_id, const InvertedIndex& index) {
    double score = 0.0;
    int doc_len = doc_lengths[doc_id];
    for (const auto& token : query_tokens) {
        auto docs = index.lookup(token);
        int df = docs.size();
        int tf = std::count(docs.begin(), docs.end(), doc_id); // For demo: 1 if present
        if (df == 0 || tf == 0) continue;
        double idf = std::log((total_docs - df + 0.5) / (df + 0.5) + 1);
        double denom = tf + k1 * (1 - b + b * doc_len / avg_doc_length);
        score += idf * (tf * (k1 + 1)) / denom;
    }
    return score;
}

int main() {
    InvertedIndex index("search_index_db");
    httplib::Server svr;

    // Build doc_lengths and stats (demo: scan all tokens)
    std::unordered_set<std::string> all_docs;
    std::unique_ptr<leveldb::Iterator> it(index.get_db()->NewIterator(leveldb::ReadOptions()));
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string token = it->key().ToString();
        std::string posting_list = it->value().ToString();
        std::istringstream iss(posting_list);
        std::string doc_id;
        while (std::getline(iss, doc_id, ',')) {
            doc_lengths[doc_id]++;
            all_docs.insert(doc_id);
        }
    }
    total_docs = all_docs.size();
    int total_len = 0;
    for (const auto& p : doc_lengths) total_len += p.second;
    avg_doc_length = total_docs ? (double)total_len / total_docs : 1.0;

    svr.Post("/search", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto j = json::parse(req.body);
            std::string query = j["query"];
            // Tokenize query (split on whitespace)
            std::istringstream iss(query);
            std::vector<std::string> tokens;
            std::string token;
            while (iss >> token) tokens.push_back(token);
            // Collect candidate docs
            std::unordered_set<std::string> candidates;
            for (const auto& t : tokens) {
                auto docs = index.lookup(t);
                candidates.insert(docs.begin(), docs.end());
            }
            // Score and rank
            std::vector<std::pair<std::string, double>> scored;
            for (const auto& doc_id : candidates) {
                double score = bm25_score(tokens, doc_id, index);
                scored.emplace_back(doc_id, score);
            }
            std::sort(scored.begin(), scored.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
            std::vector<std::string> results;
            for (const auto& p : scored) results.push_back(p.first);
            json response = { {"results", results} };
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content(std::string("{\"error\":\"") + ex.what() + "\"}", "application/json");
        }
    });

    std::cout << "C++ Search Microservice with BM25 running on http://localhost:8081" << std::endl;
    svr.listen("0.0.0.0", 8081);
    return 0;
} 