//
// Created by mc on 3/8/18.
//

#include "Packet.hpp"

void Packet::touch() {
    this->timestamp = time(NULL);
}

bool Packet::should_resend() {
    return (this->timestamp < (time(NULL) - RESEND_PERIOD));
}

Packet * Packet::generate_closed_signal(int id){
    auto * meta = new Packet_Meta();
    meta->sequence = 0;
    meta->signal = 1;
    meta->size = 0;
    meta->dispatcher = id;
    return new Packet((char *) meta, sizeof(Packet_Meta));
}

Packet * Packet::generate_ack_signal(int id, size_t offset) {
    auto * meta = new Packet_Meta();
    meta->sequence = offset;
    meta->signal = 2;
    meta->size = 0;
    meta->dispatcher = id;
    return new Packet((char *) meta, sizeof(Packet_Meta));

}

Packet * Packet::generate_meta_packet(size_t offset, size_t index, int id) {
    auto * meta = new Packet_Meta();
    meta->sequence = (int) (offset + index);
    meta->signal = 0;
    meta->size = this->length;
    meta->dispatcher = id;
    return new Packet((char *) meta, sizeof(Packet_Meta));
}

Packet * Packet::copy() {
    // must be deep copy
    char * copy = new char[this->length];
    memcpy(copy, buffer, length);
    return new Packet(copy, length);
}

Packet::Packet(char *buffer, size_t length) {
    this->buffer = buffer;
    this->length = length;
    this->timestamp = 0;
}

Packet::~Packet() {
    delete this->buffer;
}