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


    function<void()> * after_launch;
    function<void(int)> * repeat_event;

public:
    // Call back
    void operator()(ev::timer &w, int r);

    // Constructor
    Timeout_Listener(ev::default_loop *loop, double after, double repeat,
                     function<void()> * after_launch,
                     function<void(int)> * repeat_event);

};


#endif //PRPR_TIMEOUT_LISTENER_HPP
