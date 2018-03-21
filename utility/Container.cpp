//
// Created by mc on 3/7/18.
//

#include "Container.hpp"

time_t Container::get_time() {
    return time(NULL);
}

Container::Container() {
    this->mode = "undefined";
    this->clear_screen = false;
}