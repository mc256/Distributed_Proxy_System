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

    void read_callback(ev::io &w, int r);

    function<void(int, sockaddr_storage, socklen_t)> accepted_event;

    void start();

    Async_Accept(ev::default_loop *loop,
                 string address, int port);
    ~Async_Accept();
};


#endif //PRPR_ASYNC_ACCEPT_HPP
