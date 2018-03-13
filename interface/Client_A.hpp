//
// Created by mc on 2/23/18.
//

#ifndef PRPR_CLIENT_A_HPP
#define PRPR_CLIENT_A_HPP

#include "../main.hpp"

class Client_A {
private:
    // Utility
    Client_Core *core;
    ev::default_loop *loop;

    // Method
    void up_link_transmit();
    void send_signal(Packet *signal);

public:
    // Utility
    int socket_id;
    int interface_id;

    // Read
    bool holding_reader;
    Async_Read *read_handler;
    size_t read_buffer_offset;
    deque<Packet *> read_buffer;

    // Write
    Async_Write *write_handler;
    Packet *write_pointer = nullptr;
    size_t sort_buffer_offset;
    map<int, Packet *> sort_buffer;

    // Method
    void start();

    void start_writer();

    void clear_read_buffer(size_t offset);

    void terminate();

    // Constructor & Destructor
    Client_A(ev::default_loop *loop, int socket_id, int interface_id, Client_Core *core);

    ~Client_A();

    // Information
    string info();
};

#endif //PRPR_CLIENT_A_HPP
