//
// Created by mc on 3/8/18.
//

#include "Packet.hpp"

Packet::Packet(char *buffer, size_t length) {
    this->buffer = buffer;
    this->length = length;
}

Packet::~Packet() {
    delete this->buffer;
}