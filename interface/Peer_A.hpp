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
    // Start handshaking
    void start();

    // STEP 2
    // Check client's request
    void verify_client(string s);
    // Client did not pass the verification, we should send it a regular HTTP response
    void fail_verification();
    // Client passed the verification
    void pass_verification();
    // Start sending data to the client
    void start_writer();

    // Features
    // Prepare for client if it passed verification
    void prepare_for_use();

    // Constructor & Destructor
    // Constructor
    //  loop: event loop
    //  peer: contains information of the connection
    //  descriptor: socket descriptor
    //  core: a pointer to the program core
    Peer_A(ev::default_loop *loop, Proxy_Peer *peer, int descriptor, Peer_Core * core);

    // Destructor
    ~Peer_A();

    // Information
    string info();
};


#endif //PRPR_PEER_A_HPP
