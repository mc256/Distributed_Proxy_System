//
// Created by mc on 3/8/18.
//

#ifndef PRPR_PACKET_HPP
#define PRPR_PACKET_HPP

#include "../main.hpp"

class Packet {
public:
    char * buffer;
    size_t length;

    Packet(char * buffer, size_t length);
    ~Packet();
};


#endif //PRPR_PACKET_HPP
