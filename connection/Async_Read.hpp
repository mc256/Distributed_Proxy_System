//
// Created by mc on 3/7/18.
//

#ifndef PRPR_ASYNC_READ_HPP
#define PRPR_ASYNC_READ_HPP

#include "../main.hpp"

class Async_Read {
private:
    int descriptor;

    char * buffer;
    ssize_t position;
    ssize_t length;

    int timeout;
    bool undefined_length;

    ev::io read_io_watcher;
    ev::timer timeout_watcher;

public:
    void stop_watchers();

    void read_callback(ev::io &w, int r);
    void timeout_callback(ev::timer &w, int r);

    function <void(char *, ssize_t)> read_event;
    function <void(char *, ssize_t)> recv_event = nullptr;
    function <void(char *, ssize_t)> closed_event;
    function <void(char *, ssize_t)> failed_event;


    void set_timeout(int i);
    void set_undefined_length(bool b);

    void start();
    void reset();
    void reset(char *buffer, ssize_t length);

    Async_Read(ev::default_loop *loop, int descriptor);
    Async_Read(ev::default_loop *loop, int descriptor, char * buffer, ssize_t length);
    ~Async_Read();

};


#endif //PRPR_ASYNC_READ_HPP
