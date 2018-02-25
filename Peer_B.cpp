//
// Created by mc on 2/23/18.
//

#include "Peer_B.hpp"


Peer_B::Peer_B(ev::default_loop *loop, int socket_id) {
    this->loop = loop;
    this->socket_id = socket_id;
    this->active = false;

    this->dispatcher_id = ++unique_id;
    Peer_B::list[this->dispatcher_id] = this;
}


Peer_B::~Peer_B() {
    Peer_B::list.erase(this->dispatcher_id);
}

string Peer_B::info() {
    stringstream ss;
    ss << "Peer_B -\t" << socket_id << "\t" << dispatcher_id;
    ss << "\t" << "\tR:" << read_buffer.size() << "\tW:" << write_buffer.size();
    ss << "\t" << (active ? "ON" : "OFF");
    return ss.str();
}


int Peer_B::unique_id = 0;
map<int, Peer_B *> Peer_B::list = map<int, Peer_B *>();