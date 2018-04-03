//
// Created by mc on 3/9/18.
//

#ifndef PRPR_PEER_CORE_HPP
#define PRPR_PEER_CORE_HPP

#include "../main.hpp"

class Peer_Core {
private:
    ev::default_loop * loop;
    ev::timer watcher;

public:
    // Face A
    string confirm_password;
    string fake_source;

    vector<Proxy_Peer *> interface_a;
    vector<Peer_A *> connection_a;
    vector<Peer_A *> connection_a_available;
    deque<string> used_keys;

    // Face B
    string socks_address;
    int socks_port;
    map<int, Peer_B *> connection_b;

    // Method
    // Loads the configuration from file
    void load_config();
    // Start event listener
    void start();
    Peer_B * connect_socks_server(int dispatcher);

    // Call back
    void operator()(ev::timer &w, int r);

    // Constructor
    //  loop: event loop
    explicit  Peer_Core(ev::default_loop * loop);
};


#endif //PRPR_PEER_CORE_HPP
