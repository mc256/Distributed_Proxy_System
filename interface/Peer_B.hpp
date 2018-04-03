//
// Created by mc on 3/9/18.
//

#ifndef PRPR_PEER_B_HPP
#define PRPR_PEER_B_HPP

#include "../main.hpp"

class Peer_B {
private:
    // Utility
    Peer_Core *core;
    ev::default_loop *loop;

    // Method
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
    // Starts the event listener and all the connections
    void start();

    // Sends data to the SOCKS server
    void start_writer();

    // Sends data to the client application
    void down_link_transmit();

    // Release read buffer
    //  offset: the point where stop releasing
    void clear_read_buffer(size_t offset);

    // Terminate the connection
    void terminate();


    // Constructor & Destructor
    // Constructor
    //  loop: event loop
    //  interface_id: a unique number to identify the connection between SOCKS client and the client app
    //  core: a pointer to the program core
    Peer_B(ev::default_loop *loop, int interface_id, Peer_Core *core);

    // Destructor
    ~Peer_B();

    // Information
    string info();
};


#endif //PRPR_PEER_B_HPP
