//
// Created by mc on 3/9/18.
//

#ifndef PRPR_PEER_A_HPP
#define PRPR_PEER_A_HPP

#include "../main.hpp"

class Peer_A {
private:
    // Utility
    Peer_Core * core;
    ev::default_loop *loop;

    tuple<char *, size_t > generate_fake_http_response(string request);

public:
    // Utility
    int socket_id;

    // Read
    bool on_reading_data;
    Packet_Meta * read_meta = nullptr;
    Async_Read * read_handler;

    // Write
    bool on_writing;
    Async_Write * write_handler;
    deque<Packet *> write_buffer;
    Packet * write_pointer = nullptr;

    /////////////////////////////////////
    // STEP 1
    void start();

    // STEP 2
    void verify_client(string s);
    void start_writer();

    // Features
    void prepare_for_use();

    // Constructor & Destructor
    Peer_A(ev::default_loop *loop, int descriptor, Peer_Core * core);
    ~Peer_A();

    // Information
    string info();
};


#endif //PRPR_PEER_A_HPP
