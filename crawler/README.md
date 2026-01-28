# Crawler Module

## Purpose
The crawler is responsible for discovering, fetching, and processing web pages in a decentralized, peer-to-peer (P2P) manner. It forms the backbone of the search engine's data acquisition pipeline.

## Key Responsibilities
- Peer discovery and communication using a Distributed Hash Table (DHT)
- Fetching web content asynchronously
- Respecting robots.txt and domain rate limits
- Chunking and hashing content for content-addressed storage
- Publishing diffs and updates to the P2P network

## Planned Technologies
- **Language:** C++
- **Libraries:** libp2p (networking), libcurl (HTTP fetch), LevelDB (storage), SQLite (rate limiting)

## Planned Code Structure
- `p2p_dht/`: Peer discovery and messaging
- `content_store/`: Chunking, hashing, and storage
- `merkle_tree/`: Efficient diff computation
- `crawler/`: Main crawling logic, URL frontier, robots.txt handling
- `cli/`: Command-line interface for running the crawler

## Logic Overview
1. Discover peers and join the P2P network
2. Fetch URLs from the frontier, respecting robots.txt and rate limits
3. Chunk and hash fetched content, store in LevelDB
4. Compute Merkle diffs and publish updates to the network
5. Repeat until the crawl frontier is exhausted or stopped

## Next Steps
- Implement module skeletons and interfaces
- Add unit and integration tests 