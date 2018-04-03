//
// Created by mc on 3/6/18.
//

#ifndef PRPR_ASYNC_CONNECT_HPP
#define PRPR_ASYNC_CONNECT_HPP

#include "../main.hpp"

class Async_Connect {
private:
    int descriptor;

    sockaddr_in connect_address;
    socklen_t connect_address_length;

    ev::io write_io_watcher;
    ev::io read_io_watcher;
    ev::timer timeout_watcher;

    void stop_watchers();
public:
    // Event call back function
    void read_callback(ev::io &w, int r);
    void write_callback(ev::io &w, int r);
    void timeout_callback(ev::timer &w, int r);

    // The function will be called when a new connection is established
    function<void(int)> connected_event;
    // The function will be called when it is timeout or failed
    function<void()> failed_event;

    // Starts connecting the socket
    void start();


    // Constructor
    // Connect to a remote address using given address and port.
    //  loop: event loop
    //  address: string presentation of the IP address
    //  port: integer presentation of a port number
    //  timeout: number of seconds that the connection can wait for response
    Async_Connect(ev::default_loop *loop, string address, int port, int timeout);

    // Destructor
    ~Async_Connect();
};


#endif //PRPR_ASYNC_CONNECT_HPP
