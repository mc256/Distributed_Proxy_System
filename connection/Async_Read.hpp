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

    ev::io read_io_watcher;
    ev::timer timeout_watcher;

    void stop_watchers();
public:
    void read_callback(ev::io &w, int r);
    void timeout_callback(ev::timer &w, int r);

    function <void()> read_event;
    function <void()> closed_event;
    function <void()> failed_event;

    void set_timeout(int i);
    void start();

    Async_Read(ev::default_loop *loop, int descriptor, char * buffer, ssize_t length);
    ~Async_Read();

};


#endif //PRPR_ASYNC_READ_HPP
