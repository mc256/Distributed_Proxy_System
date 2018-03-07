//
// Created by mc on 2/23/18.
//

#include "Command_Listener.hpp"


void Command_Listener::callback(ev::io &w, int r) {
    count++;
    string input;
    getline(cin, input);
    cout << "stdin cb------------------------> " << input << " " << count << endl;
}

Command_Listener::Command_Listener(ev::default_loop *loop) {
    this->count = 0;
    this->watcher.set(*loop);
    this->watcher.set<Command_Listener, &Command_Listener::callback>(this);
    this->watcher.start(0, ev::READ);
}