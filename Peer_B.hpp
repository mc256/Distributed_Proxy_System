//
// Created by mc on 2/23/18.
//

#ifndef PRPR_PEER_B_HPP
#define PRPR_PEER_B_HPP

#include "main.hpp"

class Peer_B{
private:
    ev::default_loop * loop;
    int socket_id;
    int dispatcher_id;
    static int unique_id;

public:
    static map<int, Peer_B *> list;

    deque<struct Data_Package *> read_buffer;
    deque<struct Data_Package *> write_buffer;
    R_Filter * rf;
    W_Filter * wf;

    bool active;

    /////////////////////////////////////
    Peer_B(ev::default_loop * loop, int socket_id);

    ~Peer_B();

    string info();
};



#endif //PRPR_PEER_B_HPP
