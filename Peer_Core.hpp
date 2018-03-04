//
// Created by mc on 2/25/18.
//

#ifndef PRPR_PEERCORE_HPP
#define PRPR_PEERCORE_HPP

#include "main.hpp"

class Peer_Core {
private:
    // Face A

    // Face B
    vector<struct Proxy_Peer *> pp_list;

    // System
    ev::default_loop * loop;
    string address;
    int port;

    // Configure;
    int instance_id = 666;

    void up_link_transport(Peer_A * a, struct Data_Package * d, struct Data_Meta * m);
    void load_config();

public:
    Peer_Core(ev::default_loop *loop, string address, int port_begin, int port_end);
};


#endif //PRPR_PEERCORE_HPP