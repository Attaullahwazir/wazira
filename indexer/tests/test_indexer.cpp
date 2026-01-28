// indexer/tests/test_indexer.cpp
// Catch2-based test suite for the indexer pipeline
// To build: add Catch2 to your project and enable this file in CMake

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../tokenizer/tokenizer.h"
#include "../stemmer/stemmer.h"
#include "../inverted_index/inverted_index.h"
#include "../merkle_diff/merkle_diff.h"
#include <string>
#include <vector>

TEST_CASE("Full pipeline: tokenize, stem, index, lookup", "[indexer]") {
    Tokenizer tokenizer;
    Stemmer stemmer;
    InvertedIndex index("test_index_db");
    std::string doc_id = "doc1";
    std::string text = "Running runners ran easily.";
    // Tokenize
    auto tokens = tokenizer.tokenize(text);
    // Stem
    std::vector<std::string> stemmed;
    for (const auto& t : tokens) stemmed.push_back(stemmer.stem(t));
    // Index
    index.add_document(doc_id, stemmed);
    // Lookup
    for (const auto& t : stemmed) {
        auto docs = index.lookup(t);
        REQUIRE(std::find(docs.begin(), docs.end(), doc_id) != docs.end());
    }
}

TEST_CASE("Index diffing and incremental update", "[indexer]") {
    std::vector<std::string> old_tokens = {"run", "easy"};
    std::vector<std::string> new_tokens = {"run", "easier", "fast"};
    auto diff = MerkleDiff::compute_diff(old_tokens, new_tokens);
    REQUIRE(diff.size() == 2);
    REQUIRE(std::find(diff.begin(), diff.end(), "easier") != diff.end());
    REQUIRE(std::find(diff.begin(), diff.end(), "fast") != diff.end());
}

// Add more tests for batch processing, language detection, and metadata as needed. 