// tests/test_crawler.cpp
// Catch2-based test suite for the distributed crawler
// To build: add Catch2 to your project and enable this file in CMake

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../crawler/content_store/content_store.h"
#include "../crawler/merkle_tree/merkle_tree.h"
#include "../crawler/crawler/crawler.h"
#include <string>
#include <vector>

TEST_CASE("ContentStore: chunking and round-trip storage", "[content_store]") {
    ContentStore store("test_db");
    std::string data = "abcdefghijklmnopqrstuvwxyz0123456789";
    auto blocks = ContentStore::chunk_data(data, 8);
    auto hashes = store.store_blocks(blocks);
    auto retrieved = store.get_blocks(hashes);
    REQUIRE(blocks == retrieved);
}

TEST_CASE("MerkleTree: root hash and diff", "[merkle_tree]") {
    std::vector<std::string> hashes1 = {"a", "b", "c"};
    std::vector<std::string> hashes2 = {"a", "x", "c"};
    MerkleTree tree1(hashes1);
    MerkleTree tree2(hashes2);
    REQUIRE(tree1.root_hash() != tree2.root_hash());
    auto diff = tree2.diff(tree1);
    REQUIRE(diff.size() == 1);
    REQUIRE(diff[0] == "x");
}

TEST_CASE("Crawler: URL normalization", "[crawler]") {
    std::string url1 = "HTTP://Example.com/Path#fragment";
    std::string url2 = "http://example.com/Path";
    REQUIRE(Crawler::normalize_url(url1) == Crawler::normalize_url(url2));
}

TEST_CASE("Crawler: robots.txt parsing and enforcement", "[crawler]") {
    // Simulate robots.txt rules
    RobotsRules rules;
    rules.disallow = {"/private"};
    rules.allow = {"/private/open"};
    Crawler crawler("test_db", nullptr);
    std::string url1 = "http://example.com/private/page";
    std::string url2 = "http://example.com/private/open/page";
    REQUIRE(!crawler.is_allowed_by_rules(url1, rules));
    REQUIRE(crawler.is_allowed_by_rules(url2, rules));
}

// Add more integration tests for DHT, concurrency, and full crawl pipeline as needed. 