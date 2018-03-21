//
// Created by mc on 2/23/18.
//

#ifndef PRPR_CLIENT_CORE_HPP
#define PRPR_CLIENT_CORE_HPP

#include "../main.hpp"

class Client_Core{
private:
    ev::default_loop * loop;
    ev::timer watcher;

public:
    // Face A
    Async_Accept * interface_a;
    int connection_a_count;
    map<int, Client_A *> connection_a;

    string listen_address;
    int listen_port;

    // Face B
    vector<Proxy_Peer *> interface_b;
    vector<Client_B *> connection_b;
    vector<Client_B *> connection_b_available;

    string password_confirm;

    // Method
    void load_config();
    void reconnect();
    void start();

    // Call back
    void operator()(ev::timer &w, int r);

    // Constructor
    explicit  Client_Core(ev::default_loop * loop);
};


#endif //PRPR_CLIENT_CORE_HPP
