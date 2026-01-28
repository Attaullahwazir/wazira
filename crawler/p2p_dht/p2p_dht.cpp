// file: crawler/p2p_dht/p2p_dht.cpp

#include "p2p_dht.h"
#include <iostream>

using namespace std; 
namespace p2p_dht {
    // Example implementation skeletan (to be replace with real libp2p or custom logic )
    class DHTNodeImpl : public DHTNode {
        public:
           static shared_ptr<DHTNode> create(const string& listen_addr, int port) {
            return make_shared<DHTNodeImpl> (listen_addr, port);
           }

           DHTNodeImpl(const string& listen_addr, int port)
           {
            // TODO: initialize libp2p node, bind to address, etc.
            cout << "DHTNode listening on " << listen_addr <<":"<<port<<endl;
           }

           void join(const vector<string>& bootstrap_peers) override {
            // TODO: connect to bootstrap peers
           }

           void publish(const string& topic, const vector<uint8_t>& data) override {
            // TODO: publish message to Gossipsub topic
           }

           void subscribe(const string& topic, MessageCallback callback) override {
            // TODO: subscribe to Gossipsub topic
           }

           vector<string> get_peers(const string& topic) override {
            // TODO: return list of peers for topic
            return {};
           }

           void send_direct(const string& peer_id, const vector<uint8_t>& msg) override {
            // TODO: send direct message to peer
           }

           void set_encryption(bool enabled) override {
            // TODO: enable/disable encryption for all messages
           }
    };

    // factory method
    shared_ptr<DHTNode> DHTNode::create(const string& listen_addr, int port) {
        return DHTNodeImpl::create(listen_addr,port);
    }
} // namespace p2p_dht