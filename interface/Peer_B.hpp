//
// Created by mc on 3/9/18.
//

#ifndef PRPR_PEER_B_HPP
#define PRPR_PEER_B_HPP

#include "../main.hpp"

class Peer_B {
private:
    // Utility
    Peer_Core * core;
    ev::default_loop * loop;

    // Method
    void down_link_transmit();
    void send_signal(Packet *signal);

public:
    // Utility
    int socket_id;
    int interface_id;

    // Read
    Async_Read * read_handler;
    size_t read_buffer_offset;
    deque<Packet *> read_buffer;

    // Write
    Async_Write * write_handler;
    Packet * write_pointer = nullptr;
    size_t sort_buffer_offset;
    map<int, Packet *> sort_buffer;

    // Method
    void start();
    void start_writer();
    void clear_read_buffer(size_t offset);
    void terminate();


    // Constructor & Destructor
    Peer_B(ev::default_loop * loop, int interface_id, Peer_Core * core);
    ~Peer_B();

    // Information
    string info();
};


#endif //PRPR_PEER_B_HPP
