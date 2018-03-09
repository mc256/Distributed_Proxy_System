//
// Created by mc on 3/8/18.
//

#ifndef PRPR_SOCKS_CORE_HPP
#define PRPR_SOCKS_CORE_HPP

#include "../main.hpp"

class Socks_Core {
private:
    ev::default_loop * loop;

    string listen_address;
    int listen_port;

public:
    Async_Accept * proxy_server;

    // Method
    void load_config();
    void start();

    // Constructor & Destructor
    Socks_Core(ev::default_loop *loop);
};


#endif //PRPR_SOCKS_CORE_HPP
