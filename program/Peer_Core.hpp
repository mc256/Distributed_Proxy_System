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
    string listen_address;
    int listen_port_begin;  //inclusive
    int listen_port_end;    //inclusive
    string password;
    string confirm_password;
    string fake_source;

    vector<Async_Accept *> interface_a;
    vector<Peer_A *> connection_a;
    vector<Peer_A *> connection_a_available;

    // Face B
    string socks_address;
    int socks_port;
    map<int, Peer_B *> connection_b;

    // Method
    void load_config();
    void start();
    Peer_B * connect_socks_server(int dispatcher);

    // Call back
    void operator()(ev::timer &w, int r);

    // Constructor
    explicit  Peer_Core(ev::default_loop * loop);
};


#endif //PRPR_PEER_CORE_HPP
