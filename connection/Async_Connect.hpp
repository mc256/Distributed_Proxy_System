//
// Created by mc on 3/6/18.
//

#ifndef PRPR_ASYNC_CONNECT_HPP
#define PRPR_ASYNC_CONNECT_HPP

#include "../main.hpp"

class Async_Connect {

    int descriptor;

    struct sockaddr_in connect_address;
    socklen_t connect_address_length;

    ev::io write_io_watcher;
    ev::io read_io_watcher;
    ev::timer timeout_watcher;

    function<void(int)> * connected_event;
    function<void()> * failed_event;

    void stop_watchers();
public:
    void read_callback(ev::io &w, int r);
    void write_callback(ev::io &w, int r);
    void timeout_callback(ev::timer &w, int r);

    Async_Connect(ev::default_loop *loop,
                  string& address, int& port, int& timeout,
                  function<void(int)> *connected, function<void()> *failed);
};


#endif //PRPR_ASYNC_CONNECT_HPP
