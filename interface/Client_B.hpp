//
// Created by mc on 2/23/18.
//

#ifndef PRPR_CLIENT_B_HPP
#define PRPR_CLIENT_B_HPP

#include "../main.hpp"

class Client_B{
private:
    void generate_fake_request();

public:
    // Utility
    Client_Core * core;
    ev::default_loop * loop;

    Proxy_Peer * peer;
    int interface_id;

    // Read
    Async_Read * read_handler;

    // Write
    Async_Write * write_handler;

    /////////////////////////////////////
    // STEP 1
    void start();

    // STEP 2
    void verify_peer();

    // STEP 3
    void split_package();

    // STEP 4
    void shutdown();

    // Constructor & Destructor
    Client_B(ev::default_loop * loop, Proxy_Peer * peer, int interface_id, Client_Core * core);
    ~Client_B();

    // Information
    string info();

};


#endif //PRPR_CLIENT_B_HPP
