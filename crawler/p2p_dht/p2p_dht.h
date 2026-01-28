// File: crawler/p2p_dht/p2p_dht.h
// distributed hash table
#ifndef P2P_DHT_H
#define P2P_DHT_H

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace p2p_dht {

    // Callback for received messages
    using MessageCallback = std::function<void(const std::string& from_peer, const std::vector<uint8_t>& data)>;

    class DHTNode {
    public:
        // Create and bootstrap a DHT node
        static std::shared_ptr<DHTNode> create(const std::string& listen_addr, int port);

        // Join the network using a list of bootstrap peers
        virtual void join(const std::vector<std::string>& bootstrap_peers) = 0;

        // Publish a message to a topic (Gossipsub)
        virtual void publish(const std::string& topic, const std::vector<uint8_t>& data) = 0;

        // Subscribe to a topic (Gossipsub)
        virtual void subscribe(const std::string& topic, MessageCallback callback) = 0;

        // Get a list of peers for a topic
        virtual std::vector<std::string> get_peers(const std::string& topic) = 0;

        // Send a direct message to a peer
        virtual void send_direct(const std::string& peer_id, const std::vector<uint8_t>& msg) = 0;

        // Enable/disable encryption for all messages
        virtual void set_encryption(bool enabled) = 0;

        virtual ~DHTNode() = default;
    };

} // namespace p2p_dht

#endif // P2P_DHT_H 