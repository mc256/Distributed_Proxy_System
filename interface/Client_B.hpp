//
// Created by mc on 2/23/18.
//

#ifndef PRPR_CLIENT_B_HPP
#define PRPR_CLIENT_B_HPP

#include "../main.hpp"

class Client_B{
private:
    // Utility
    Client_Core * core;
    ev::default_loop * loop;

    tuple<char *, size_t > generate_fake_request();

public:
    // Utility
    Proxy_Peer * peer;
    int socket_id;
    string key;
    Encryption * coder;

    // Read
    bool on_reading_data; // false = meta; true = data
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
    // Verify if the response from peer is correct
    //  buf: pointer to the buffer that contains the response
    //  s: size of the buffer
    void verify_peer(char *buf, ssize_t s);

    // Features
    // Prepare for proxy data if it passed verification
    void prepare_for_use();
    // Start sending data to the proxy server
    void start_writer();

    // Constructor & Destructor
    // Constructor
    //  loop: event loop
    //  peer: contains information of the connection
    //  socket_id: socket descriptor
    //  core: a pointer to the program core
    Client_B(ev::default_loop * loop, Proxy_Peer * peer, int socket_id, Client_Core * core);

    // Destructor
    ~Client_B();

    // Information
    string info();


};


#endif //PRPR_CLIENT_B_HPP
