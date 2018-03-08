//
// Created by mc on 2/23/18.
//

#ifndef PRPR_HANDSHAKE_HPP
#define PRPR_HANDSHAKE_HPP

#include "../main.hpp"

class Handshake {

private:
    ev::default_loop * loop;
    int p2c_descriptor;

public:
    void start();
    Handshake(ev::default_loop * loop, int p2c_descriptor);

};


#endif //PRPR_HANDSHAKE_HPP
