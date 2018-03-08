//
// Created by mc on 2/23/18.
//

#include "Timeout_Listener.hpp"

#include <utility>


void Timeout_Listener::operator()(ev::timer &w, int r) {
    if (count == 0) {
        this->after_launch(this->c);
    } else {
        this->repeat_event(this->c, this->count);
    }
    count ++;
}

// Constructor
Timeout_Listener::Timeout_Listener(ev::default_loop *loop, double after, double repeat) {
    this->count = 0;
    this->after_launch = after_launch;
    this->repeat_event = repeat_event;
    this->c = new Container();

    this->watcher.set(*loop);
    this->watcher.set(this);
    this->watcher.start(after, repeat);

}
