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
    time_t timestamp;

    void touch();
    bool should_resend();
    static Packet * generate_closed_signal(int id);
    static Packet * generate_ack_signal(int id, size_t offset);
    Packet * generate_meta_packet(size_t offset, size_t index, int id);
    Packet * copy();

    Packet(char * buffer, size_t length);
    ~Packet();
};


#endif //PRPR_PACKET_HPP
