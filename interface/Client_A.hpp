//
// Created by mc on 2/23/18.
//

#ifndef PRPR_CLIENT_A_HPP
#define PRPR_CLIENT_A_HPP

#include "../main.hpp"

class Client_A{
public:
    // Utility
    Client_Core * core;
    ev::default_loop * loop;

    int socket_id;
    int interface_id;

    // Read
    Async_Read * read_handler;
    size_t read_buffer_offset;
    deque<Packet *> read_buffer;

    // Write
    Async_Write * write_handler;
    size_t sort_buffer_offset;
    map<int, Packet *> sort_buffer;

    // Constructor & Destructor
    Client_A(ev::default_loop * loop, int socket_id, int interface_id, Client_Core * core);
    ~Client_A();

    // Information
    string info();
};

#endif //PRPR_CLIENT_A_HPP
