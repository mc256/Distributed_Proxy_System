//
// Created by mc on 3/7/18.
//

#ifndef PRPR_ASYNC_ACCEPT_HPP
#define PRPR_ASYNC_ACCEPT_HPP

#include "../main.hpp"

class Async_Accept {
private:
    int descriptor;

    sockaddr_in accept_address;
    socklen_t accept_address_length;

    ev::io read_io_watcher;

public:
    // Event call back function
    void read_callback(ev::io &w, int r);

    // The function will be called when a new connection is established
    function<void(int, sockaddr_storage, socklen_t)> accepted_event;

    // Starts the socket listener
    void start();

    // Constructor
    // Creates a socket listener using given address and port
    //  loop: event loop
    //  address: string presentation of the IP address
    //  port: integer presentation of a port number
    Async_Accept(ev::default_loop *loop, string address, int port);

    // Destructor
    ~Async_Accept();
};


#endif //PRPR_ASYNC_ACCEPT_HPP
