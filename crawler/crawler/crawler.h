#ifndef CRAWLER_H
#define CRAWLER_H

#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <memory>
#include <mutex>
#include "../content_store/content_store.h"
#include "../merkle_tree/merkle_tree.h"
#include <unordered_map>
#include <chrono>
#include <thread>
#include "../p2p_dht/p2p_dht.h"
#include "include/inverted_index.h"

struct RobotsRules {
    std::vector<std::string> disallow;
    std::vector<std::string> allow;
};

class Crawler {
public:
    Crawler(const std::string& db_path, std::shared_ptr<p2p_dht::DHTNode> dht_node);

    void add_seed_urls(const std::vector<std::string>& urls);
    void run_concurrent(int num_threads = 4, int max_pages = 0);
    void add_url(const std::string& url);
    std::string normalize_url(const std::string& url) const;
    void dht_publish_url(const std::string& url);
    std::vector<std::string> dht_receive_urls();
    void fetch_and_process(const std::string& url);
    bool allowed_by_robots(const std::string& url) const;
    void publish_diff(const std::string& domain, const MerkleTree& old_tree, const MerkleTree& new_tree) const;
    void set_domain_delay(int ms);
    void extract_and_enqueue_links(const std::string& html, const std::string& base_url);
    std::string resolve_url(const std::string& link, const std::string& base_url) const;
    static void log(const std::string& msg);
    void set_indexer(InvertedIndex* indexer);

private:
    std::unique_ptr<ContentStore> content_store_;
    std::shared_ptr<p2p_dht::DHTNode> dht_node_;
    std::string dht_topic_ = "urls";
    std::queue<std::string> frontier_;
    std::unordered_set<std::string> seen_urls_;
    std::mutex frontier_mutex_;
    std::unordered_map<std::string, RobotsRules> robots_cache_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> last_fetch_time_;
    int domain_delay_ms_ = 1000;
    std::mutex seen_mutex_;
    std::mutex fetch_time_mutex_;
    InvertedIndex* indexer_ = nullptr;

    bool fetch_url(const std::string& url, std::string& out_content);
    bool fetch_and_cache_robots(const std::string& domain);
    bool is_allowed_by_rules(const std::string& url, const RobotsRules& rules) const;
    static std::string extract_domain(const std::string& url);
    void enforce_domain_delay(const std::string& domain);
};

#endif // CRAWLER_H