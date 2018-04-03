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

    // Sends data to the server application
    void up_link_transmit();
    // Sends data to the SOCKS client
    void start_writer();

    // Release read buffer
    //  offset: the point where stop releasing
    void clear_read_buffer(size_t offset);

    // Terminate the connection
    void terminate();

    // Constructor & Destructor
    // Constructor
    //  loop: event loop
    //  socket_id: socket descriptor
    //  interface_id: a unique number to identify the connection between SOCKS client and the client app
    //  core: a pointer to the program core
    Client_A(ev::default_loop *loop, int socket_id, int interface_id, Client_Core *core);

    // Destructor
    ~Client_A();

    // Information
    string info();
};

#endif //PRPR_CLIENT_A_HPP
