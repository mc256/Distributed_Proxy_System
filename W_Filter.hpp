//
// Created by mc on 2/23/18.
//

#ifndef PRPR_W_FILTER_HPP
#define PRPR_W_FILTER_HPP

#include "main.hpp"


class W_Filter {
    // This Filter only handle outgoing data (WRITE)
private:
    ev::default_loop *loop;
    ev::io event_listener;

    bool keep;
public:
    int offset;
    int socket_id;
    W_Filter **back_link;

    deque<struct Data_Package *> *buffer;
    struct Data_Package *current;

    function<void(W_Filter *)> hook_closed;
    function<void(W_Filter *)> hook_finished;
    function<void(W_Filter *)> hook_sent;

    void operator()(ev::io &watcher, int revents);

    void start();

    void set_keep(bool b);

    W_Filter(ev::default_loop *loop, deque<struct Data_Package *> *buffer, int socket_id, W_Filter **back_link);

    void active_terminate();
    void passive_terminate();
};


#endif //PRPR_W_FILTER_HPP
