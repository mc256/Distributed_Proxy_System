//
// Created by mc on 2/23/18.
//

#ifndef PRPR_CLIENT_CORE_HPP
#define PRPR_CLIENT_CORE_HPP

#include "../main.hpp"

class Client_Core{
public:
    ev::default_loop * loop;

    // Face A
    Async_Accept * interface_a;
    int connection_a_count;
    map<int, Client_A *> connection_a;

    string listen_address;
    int listen_port;

    // Face B
    vector<Proxy_Peer *> interface_b;
    int connection_b_count;
    vector<Client_B *> connection_b;
    vector<Client_B *> connection_b_available;

    string password_confirm;

    // Configure
    void load_config();

    // Constructor
    explicit  Client_Core(ev::default_loop * loop);
};


#endif //PRPR_CLIENT_CORE_HPP
