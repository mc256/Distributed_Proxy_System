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

    tuple<char *, size_t > generate_fake_http_response();

public:
    // Utility
    Proxy_Peer * peer;
    int socket_id;
    string key;
    Encryption * coder;

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
    void fail_verification();
    void pass_verification();
    void start_writer();

    // Features
    void prepare_for_use();

    // Constructor & Destructor
    Peer_A(ev::default_loop *loop, Proxy_Peer *peer, int descriptor, Peer_Core * core);
    ~Peer_A();

    // Information
    string info();
};


#endif //PRPR_PEER_A_HPP
