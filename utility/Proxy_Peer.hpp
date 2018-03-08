//
// Created by mc on 3/8/18.
//

#ifndef PRPR_PROXY_PEER_HPP
#define PRPR_PROXY_PEER_HPP

#include "../main.hpp"

class Proxy_Peer {
public:
    bool connected;
    bool available;

    string address;
    int port;

    string password;

    int descriptor;

    Async_Connect * interface;

    Proxy_Peer(string address, int port, string password);

};


#endif //PRPR_PROXY_PEER_HPP
