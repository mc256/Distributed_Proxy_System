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
    void start();

    // STEP 2
    void verify_peer(char *buf, ssize_t s);

    // Features
    void prepare_for_use();
    void start_writer();

    // Constructor & Destructor
    Client_B(ev::default_loop * loop, Proxy_Peer * peer, int socket_id, Client_Core * core);
    ~Client_B();

    // Information
    string info();


};


#endif //PRPR_CLIENT_B_HPP
