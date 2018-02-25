//
// Created by mc on 2/23/18.
//

#ifndef PRPR_COMMAND_LISTENER_HPP
#define PRPR_COMMAND_LISTENER_HPP

#include "main.hpp"

class Command_Listener {
private:
    ev::io watcher;
    int count;

    void callback(ev::io &watcher, int revents);

public:
    Command_Listener(ev::default_loop *loop);
};


#endif //PRPR_COMMAND_LISTENER_HPP
