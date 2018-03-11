//
// Created by mc on 3/7/18.
//

#ifndef PRPR_ASYNC_WRITE_HPP
#define PRPR_ASYNC_WRITE_HPP

#include "../main.hpp"

class Async_Write {
private:
    int descriptor;

    char * buffer;
    ssize_t position;
    ssize_t length;

    int timeout;

    ev::io write_io_watcher;
    ev::timer timeout_watcher;

    void stop_watchers();
public:
    void write_callback(ev::io &w, int r);
    void timeout_callback(ev::timer &w, int r);

    function <void(char *, ssize_t)> wrote_event;
    function <void(char *, ssize_t)> closed_event;
    function <void(char *, ssize_t)> failed_event;

    void set_timeout(int i);
    void start();
    void reset();
    void reset(char * buffer, ssize_t length);

    Async_Write(ev::default_loop *loop, int descriptor);
    Async_Write(ev::default_loop *loop, int descriptor, char * buffer, ssize_t length);
    ~Async_Write();

};


#endif //PRPR_ASYNC_WRITE_HPP
