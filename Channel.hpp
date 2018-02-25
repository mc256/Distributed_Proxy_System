//
// Created by mc on 2/23/18.
//

#ifndef PRPR_CHANNEL_HPP
#define PRPR_CHANNEL_HPP

#include "main.hpp"

class Channel {
private:
    ev::default_loop *loop;
    ev::io read_io_watcher;
    ev::io write_io_watcher;

    int read_descriptor;
    int write_descriptor;

    UCHAR buffer[MAX_BUFFER_SIZE];
    int buffer_length;
    int buffer_sent;

    Channel *pair;

public:
    void terminate();
    void passive_terminate();
    void active_terminate();

    void recv_callback(ev::io &watcher, int revents);
    void send_callback(ev::io &watcher, int revents);

    void setPair(Channel *anotherChannel);

    void start();

    Channel(ev::default_loop *loop, int read, int write);

};


#endif //PRPR_CHANNEL_HPP
