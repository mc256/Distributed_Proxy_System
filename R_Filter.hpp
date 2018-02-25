//
// Created by mc on 2/23/18.
//

#ifndef PRPR_R_FILTER_HPP
#define PRPR_R_FILTER_HPP

#include "main.hpp"

class R_Filter {
    // This Filter only handle incoming data (READ)
private:
    ev::default_loop *loop;
    ev::io event_listener;

public:
    int offset;
    int socket_id;
    int dispatcher_id;
    R_Filter **back_link;

    deque<struct Data_Package *> *buffer;

    function<void(R_Filter *)> hook_closed;
    function<void(R_Filter *)> hook_recv;

    void operator()(ev::io &watcher, int revents);

    void start();

    R_Filter(ev::default_loop *loop, deque<struct Data_Package *> *buffer, int socket_id, R_Filter **back_link,
             int dispatcher_id);

    void active_terminate();
    void passive_terminate();
};

#endif //PRPR_R_FILTER_HPP
