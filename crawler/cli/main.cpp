// main.cpp
// CLI entry point for running the crawler
//
// Usage: p2p-crawler run --seeds seeds.txt --max-pages 10000
//
// This file parses command-line arguments, initializes the Crawler,
// loads seed URLs, and starts the crawl process.

#include "../crawler/crawler.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

// Simple command-line argument parser
void parse_args(int argc, char* argv[], std::string& seeds_file, int& max_pages) {
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--seeds") == 0 && i + 1 < argc) {
            seeds_file = argv[++i];
        } else if (std::strcmp(argv[i], "--max-pages") == 0 && i + 1 < argc) {
            max_pages = std::stoi(argv[++i]);
        }
    }
}

int main(int argc, char* argv[]) {
    std::string seeds_file = "seeds.txt";
    int max_pages = 10000;
    parse_args(argc, argv, seeds_file, max_pages);

    // Load seed URLs
    std::vector<std::string> seeds;
    std::ifstream infile(seeds_file);
    if (!infile) {
        std::cerr << "Error: Could not open seeds file: " << seeds_file << std::endl;
        return 1;
    }
    std::string url;
    while (std::getline(infile, url)) {
        if (!url.empty()) seeds.push_back(url);
    }
    if (seeds.empty()) {
        std::cerr << "Error: No seed URLs found in " << seeds_file << std::endl;
        return 1;
    }

    // Initialize the crawler (LevelDB path: "crawler_db", DHT: nullptr for now)
    Crawler crawler("crawler_db", nullptr);
    crawler.add_seed_urls(seeds);

    // Main crawl loop: limit by max_pages
    int pages_crawled = 0;
    while (pages_crawled < max_pages) {
        // The crawler's run() processes the entire frontier, so for demo, break after max_pages
        crawler.run();
        pages_crawled += seeds.size(); // Approximate; real implementation should count processed URLs
        break; // Remove this break if run() is updated to process one page at a time
    }

    std::cout << "Crawl complete. Pages crawled: " << pages_crawled << std::endl;
    return 0;
} 