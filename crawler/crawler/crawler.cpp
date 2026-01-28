#pragma message("Building crawler.cpp from " __FILE__)
#include "crawler.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include <regex>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <ctime>
#include "../p2p_dht/p2p_dht.h"
#include "include/tokenizer.h"
#include "include/stemmer.h"
#include <atomic>
/**
 * @brief Construct a Crawler instance with configuration and DHT node.
 */
Crawler::Crawler(const std::string& db_path, std::shared_ptr<p2p_dht::DHTNode> dht_node)
    : content_store_(std::make_unique<ContentStore>(db_path)), dht_node_(dht_node) {}

/**
 * @brief Add seed URLs to the crawl frontier (thread-safe).
 */
void Crawler::add_seed_urls(const std::vector<std::string>& urls) {
    std::lock_guard<std::mutex> lock(frontier_mutex_);
    for (const auto& url : urls) {
        if (seen_urls_.find(url) == seen_urls_.end()) {
            frontier_.push(url);
            seen_urls_.insert(url);
        }
    }
}

/**
 * @brief Main crawling loop. Fetches URLs from the frontier and processes them.
 *        This is a simplified, single-threaded version for clarity.
 */
void Crawler::run() {
    while (true) {
        std::string url;
        {
            std::lock_guard<std::mutex> lock(frontier_mutex_);
            if (frontier_.empty()) {
                std::cout << "Frontier empty. Crawling complete." << std::endl;
                break;
            }
            url = frontier_.front();
            frontier_.pop();
        }
        fetch_and_process(url);
        // Sleep to avoid hammering servers (replace with rate limiter)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

/**
 * @brief Helper function for libcurl to write fetched data to a std::string.
 */
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/**
 * @brief Fetch the content of a URL using libcurl.
 */
bool Crawler::fetch_url(const std::string& url, std::string& out_content) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_content);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 10 second timeout
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return (res == CURLE_OK);
}

/**
 * @brief Set the minimum delay (in milliseconds) between requests to the same domain.
 */
void Crawler::set_domain_delay(int ms) {
    domain_delay_ms_ = ms;
}

/**
 * @brief Check and enforce domain rate limiting. Sleeps if needed.
 */
void Crawler::enforce_domain_delay(const std::string& domain) {
    auto now = std::chrono::steady_clock::now();
    auto it = last_fetch_time_.find(domain);
    if (it != last_fetch_time_.end()) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second).count();
        if (elapsed < domain_delay_ms_) {
            int sleep_ms = domain_delay_ms_ - static_cast<int>(elapsed);
            if (sleep_ms > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
            }
        }
    }
    last_fetch_time_[domain] = std::chrono::steady_clock::now();
}

/**
 * @brief Fetch and process a single URL: fetch, chunk, store, build Merkle tree, publish diff.
 */
void Crawler::fetch_and_process(const std::string& url) {
    try {
        log("Fetching: " + url);
        if (!allowed_by_robots(url)) {
            log("Blocked by robots.txt: " + url);
            return;
        }
        std::string domain = extract_domain(url);
        enforce_domain_delay(domain);
        std::string html;
        if (!fetch_url(url, html)) {
            log("Failed to fetch: " + url);
            return;
        }
        // Chunk and store content
        auto blocks = ContentStore::chunk_data(html);
        auto hashes = content_store_->store_blocks(blocks);
        // Build Merkle tree
        MerkleTree new_tree(hashes);
        MerkleTree old_tree({});
        publish_diff(url, old_tree, new_tree);
        log("Root hash for " + url + ": " + new_tree.root_hash());
        // Index content if indexer is set
        if (indexer_) {
            // For demo: use URL as doc_id, tokenize and stem content
            Tokenizer tokenizer;
            Stemmer stemmer;
            auto tokens = tokenizer.tokenize(html);
            std::vector<std::string> stemmed;
            for (const auto& t : tokens) stemmed.push_back(stemmer.stem(t));
            indexer_->add_document(url, stemmed);
            log("Indexed content for: " + url);
        }
        // Extract and enqueue links
        extract_and_enqueue_links(html, url);
    } catch (const std::exception& ex) {
        log(std::string("Exception in fetch_and_process: ") + ex.what());
    } catch (...) {
        log("Unknown exception in fetch_and_process");
    }
}

/**
 * @brief Extract the domain from a URL (simple version, http[s]://domain/...)
 */
std::string Crawler::extract_domain(const std::string& url) {
    std::regex re(R"(^https?://([^/]+)/?)");
    std::smatch match;
    if (std::regex_search(url, match, re) && match.size() > 1) {
        return match[1];
    }
    return "";
}

/**
 * @brief Fetch and parse robots.txt for a domain, updating the cache.
 *        Only supports Disallow/Allow for User-agent: * (basic version).
 */
bool Crawler::fetch_and_cache_robots(const std::string& domain) {
    std::string robots_url = "http://" + domain + "/robots.txt";
    std::string content;
    if (!fetch_url(robots_url, content)) {
        // Try https if http fails
        robots_url = "https://" + domain + "/robots.txt";
        if (!fetch_url(robots_url, content)) {
            return false;
        }
    }
    RobotsRules rules;
    std::istringstream iss(content);
    std::string line;
    bool relevant = false;
    while (std::getline(iss, line)) {
        // Remove comments and trim
        auto comment = line.find('#');
        if (comment != std::string::npos) line = line.substr(0, comment);
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (line.empty()) continue;
        if (line.find("User-agent:") == 0) {
            std::string agent = line.substr(11);
            agent.erase(0, agent.find_first_not_of(" \t"));
            relevant = (agent == "*" || agent.empty());
        } else if (relevant && line.find("Disallow:") == 0) {
            std::string path = line.substr(9);
            path.erase(0, path.find_first_not_of(" \t"));
            if (!path.empty()) rules.disallow.push_back(path);
        } else if (relevant && line.find("Allow:") == 0) {
            std::string path = line.substr(6);
            path.erase(0, path.find_first_not_of(" \t"));
            if (!path.empty()) rules.allow.push_back(path);
        } else if (line.find("User-agent:") == 0) {
            relevant = false;
        }
    }
    robots_cache_[domain] = rules;
    return true;
}

/**
 * @brief Check if a URL is allowed by cached robots.txt rules (basic path matching).
 */
bool Crawler::is_allowed_by_rules(const std::string& url, const RobotsRules& rules) const {
    // Extract path from URL
    std::regex re(R"(^https?://[^/]+(/.*)?$");
    std::smatch match;
    std::string path = "/";
    if (std::regex_search(url, match, re) && match.size() > 1 && match[1].matched) {
        path = match[1];
    }
    // Allow rules take precedence over disallow
    for (const auto& allow : rules.allow) {
        if (path.find(allow) == 0) return true;
    }
    for (const auto& dis : rules.disallow) {
        if (path.find(dis) == 0) return false;
    }
    return true;
}

/**
 * @brief Check if a URL is allowed by robots.txt (fetches and caches as needed).
 */
bool Crawler::allowed_by_robots(const std::string& url) const {
    std::string domain = extract_domain(url);
    if (domain.empty()) return false;
    // Check cache
    auto it = robots_cache_.find(domain);
    if (it == robots_cache_.end()) {
        if (!fetch_and_cache_robots(domain)) {
            // If robots.txt cannot be fetched, default to allow
            return true;
        }
        it = robots_cache_.find(domain);
    }
    return is_allowed_by_rules(url, it->second);
}

/**
 * @brief Publish Merkle diff for a domain via P2P DHT (placeholder).
 *        Real implementation should serialize and send diff packet.
 */
void Crawler::publish_diff(const std::string& domain, const MerkleTree& old_tree, const MerkleTree& new_tree) const {
    // TODO: Implement P2P DHT diff publishing
    std::cout << "Publishing diff for domain: " << domain << std::endl;
    std::cout << "Old root: " << old_tree.root_hash() << ", New root: " << new_tree.root_hash() << std::endl;
    auto diffs = new_tree.diff(old_tree);
    std::cout << "Updated hashes: [";
    for (const auto& h : diffs) std::cout << h << ", ";
    std::cout << "]" << std::endl;
}

/**
 * @brief Normalize a URL (remove fragments, lowercase host, etc.).
 */
std::string Crawler::normalize_url(const std::string& url) const{
    // Basic normalization: lowercase scheme/host, remove fragment
    std::regex re(R"(^([a-zA-Z]+)://([^/#?]+)([^#]*)#?.*$)");
    std::smatch match;
    if (std::regex_match(url, match, re)) {
        std::string scheme = match[1];
        std::string host = match[2];
        std::string path = match[3];
        std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);
        std::transform(host.begin(), host.end(), host.begin(), ::tolower);
        return scheme + "://" + host + path;
    }
    return url;
}

/**
 * @brief Add a single URL to the crawl frontier (normalized, deduplicated, thread-safe).
 */
void Crawler::add_url(const std::string& url) {
    std::string norm = normalize_url(url);
    std::lock_guard<std::mutex> lock(frontier_mutex_);
    std::lock_guard<std::mutex> seen_lock(seen_mutex_);
    if (seen_urls_.find(norm) == seen_urls_.end()) {
        frontier_.push(norm);
        seen_urls_.insert(norm);
    }
}

/**
 * @brief Placeholder: Publish a URL to the DHT (to be implemented).
 */
void Crawler::dht_publish_url(const std::string& url) {
    if (!dht_node_) return;
    std::vector<uint8_t> data(url.begin(), url.end());
    try {
        dht_node_->publish(dht_topic_, data);
        log("Published URL to DHT: " + url);
    } catch (const std::exception& ex) {
        log(std::string("DHT publish error: ") + ex.what());
    }
}

/**
 * @brief Placeholder: Receive URLs from the DHT (to be implemented).
 */
std::vector<std::string> Crawler::dht_receive_urls() {
    // No longer needed: DHT subscription is now event-driven
    return {};
}

/**
 * @brief Start the crawling process with multiple threads.
 */
void Crawler::run_concurrent(int num_threads, int max_pages) {
    std::atomic<int> pages_crawled{0};
    // Subscribe to DHT topic for URLs
    if (dht_node_) {
        dht_node_->subscribe(dht_topic_, [this](const std::string& from_peer, const std::vector<uint8_t>& data) {
            std::string url(data.begin(), data.end());
            log("Received URL from DHT: " + url);
            add_url(url);
        });
    }
    auto worker = [&]() {
        while (true) {
            std::string url;
            {
                std::lock_guard<std::mutex> lock(frontier_mutex_);
                if (frontier_.empty() || (max_pages > 0 && pages_crawled >= max_pages)) {
                    break;
                }
                url = frontier_.front();
                frontier_.pop();
            }
            fetch_and_process(url);
            ++pages_crawled;
        }
    };
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker);
    }
    for (auto& t : threads) {
        t.join();
    }
    log("Concurrent crawl complete. Pages crawled: " + std::to_string(pages_crawled));
}

/**
 * @brief Extract and enqueue all valid HTTP(S) links from HTML content.
 */
void Crawler::extract_and_enqueue_links(const std::string& html, const std::string& base_url) {
    std::regex href_re(R"(<a\s+[^>]*href=["']([^"'#>]+)["'])", std::regex::icase);
    auto begin = std::sregex_iterator(html.begin(), html.end(), href_re);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        std::string link = (*it)[1];
        std::string abs_url = resolve_url(link, base_url);
        if (abs_url.empty()) continue;
        if (abs_url.find("http://") == 0 || abs_url.find("https://") == 0) {
            add_url(abs_url);
            log("Discovered and enqueued: " + abs_url);
            // DHT integration: publish discovered URL
            dht_publish_url(abs_url);
        }
    }
}

/**
 * @brief Log a message with thread ID and timestamp.
 */
void Crawler::log(const std::string& msg) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &now_c);
#else
    localtime_r(&now_c, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%F %T") << " [TID " << std::this_thread::get_id() << "] " << msg << std::endl;
    std::cout << oss.str();
}

/**
 * @brief Resolve a possibly relative URL to an absolute URL using a base URL.
 *        Handles protocol-relative, root-relative, and relative paths.
 */
std::string Crawler::resolve_url(const std::string& link, const std::string& base_url) const{
    if (link.empty()) return "";
    // Absolute URL
    if (link.find("http://") == 0 || link.find("https://") == 0) {
        return Crawler::normalize_url(link);
    }
    // Protocol-relative (//example.com)
    if (link.find("//") == 0) {
        std::regex re(R"(^([a-zA-Z]+):)");
        std::smatch match;
        if (std::regex_search(base_url, match, re) && match.size() > 1) {
            return Crawler::normalize_url(match[1].str() + ":" + link);
        }
        return "http:" + link;
    }
    // Root-relative (/path)
    if (link[0] == '/') {
        std::regex re(R"(^([a-zA-Z]+://[^/]+))");
        std::smatch match;
        if (std::regex_search(base_url, match, re) && match.size() > 1) {
            return Crawler::normalize_url(match[1].str() + link);
        }
        return "";
    }
    // Relative path (../, ./, etc.)
    std::regex re(R"(^([a-zA-Z]+://[^/]+)(/.*)?$)");
    std::smatch match;
    if (std::regex_search(base_url, match, re) && match.size() > 1) {
        std::string base = match[1];
        std::string path = match.size() > 2 && match[2].matched ? match[2].str() : "/";
        auto last_slash = path.find_last_of('/');
        if (last_slash != std::string::npos) {
            path = path.substr(0, last_slash + 1);
        }
        std::string full = base + path + link;
        std::vector<std::string> parts;
        std::istringstream iss(full);
        std::string token;
        while (std::getline(iss, token, '/')) {
            if (token == "." || token.empty()) continue;
            if (token == ".." && !parts.empty()) { parts.pop_back(); continue; }
            if (token != "..") parts.push_back(token);
        }
        std::ostringstream oss;
        oss << base_url.substr(0, base_url.find("://") + 3);
        oss << base.substr(base_url.find("://") + 3);
        for (const auto& p : parts) oss << "/" << p;
        return normalize_url(oss.str());
    }
    return "";
}
// std::string Crawler::resolve_url(const std::string& link, const std::string& base_url) const {
//     // TODO: Implement actual URL resolution logic
//     // For now, just return the link as-is
//     return link;
// }
void Crawler::set_indexer(InvertedIndex* indexer) {
    indexer_ = indexer;
}

// TEST SKELETONS (to be implemented in test files)
// void test_content_store();
// void test_merkle_tree();
// void test_url_normalization();
// void test_crawler_basic();
