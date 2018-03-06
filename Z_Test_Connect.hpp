//
// Created by mc on 3/6/18.
//

#ifndef PRPR_Z_TEST_CONNECT_HPP
#define PRPR_Z_TEST_CONNECT_HPP

#include "main.hpp"


class Z_Test_Connect {
private:
    int descriptor;

    struct sockaddr_in connect_address;
    socklen_t connect_address_length;

    ev::io write_io_watcher;
    ev::io read_io_watcher;

public:
    void operator()(ev::io &watcher, int revents);
    void write_cb(ev::io &watcher, int revents);

    Z_Test_Connect(ev::default_loop * loop, string address, int port);

};


#endif //PRPR_Z_TEST_CONNECT_HPP
