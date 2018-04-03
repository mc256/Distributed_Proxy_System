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
    // Loads the configuration from file
    void load_config();
    // Start event listener
    void start();

    // Constructor & Destructor
    //  loop: event loop
    Socks_Core(ev::default_loop *loop);
};


#endif //PRPR_SOCKS_CORE_HPP
