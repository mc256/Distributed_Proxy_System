//
// Created by mc on 2/25/18.
//

#ifndef PRPR_PEERCORE_HPP
#define PRPR_PEERCORE_HPP

#include "main.hpp"

class Peer_Core {
private:
    // Face A
    Server_Accept<Peer_A> * sa_pa;

    // Face B
    vector<struct Proxy_Peer *> pp_list;

    // System
    ev::default_loop * loop;
    string address;
    int port;

    // Configure;
    int instance_id = 666;

public:
    Peer_Core(ev::default_loop *loop);
};


#endif //PRPR_PEERCORE_HPP
