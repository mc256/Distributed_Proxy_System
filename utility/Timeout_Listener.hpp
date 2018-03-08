//
// Created by mc on 2/23/18.
//


#ifndef PRPR_TIMEOUT_LISTENER_HPP
#define PRPR_TIMEOUT_LISTENER_HPP

#include "../main.hpp"

class Timeout_Listener {
private:
    ev::timer watcher;
    int count;

    Container *c;

public:

    function<void(Container *)> after_launch;
    function<void(Container *, int)> repeat_event;

    // Call back
    void operator()(ev::timer &w, int r);

    // Constructor
    Timeout_Listener(ev::default_loop *loop, double after, double repeat);

};


#endif //PRPR_TIMEOUT_LISTENER_HPP
