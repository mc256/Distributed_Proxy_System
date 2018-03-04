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
    int seq_up, seq_down;
    struct Data_Meta meta;

    void attach_meta();
public:
    static map<int, Peer_B *> interface_list;
    static vector<Peer_B *> available_list;

    struct Proxy_Peer * peer;

    deque<struct Data_Package *> read_buffer;
    deque<struct Data_Package *> write_buffer;
    map<int,struct Data_Package *> write_sort_pool;

    R_Filter * rf;
    W_Filter * wf;

    bool active;

    static function<void (Peer_B *, struct Data_Package *)> hook_core_recv;

    void start();
    void flush_sort_pool();

    /////////////////////////////////////
    Peer_B(ev::default_loop * loop, struct Proxy_Peer * peer, int dispatcher_id);

    ~Peer_B();

    string info();
};



#endif //PRPR_PEER_B_HPP
