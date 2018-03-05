//
// Created by mc on 2/23/18.
//

#ifndef PRPR_PEER_A_HPP
#define PRPR_PEER_A_HPP

#include "main.hpp"

class Peer_A{
private:
    ev::default_loop * loop;
    int socket_id;
    int dispatcher_id;
    static int unique_id;

    string password;
public:
    static map<int, Peer_A *> interface_list;
    static vector<Peer_A *> available_list;

    bool fake_header_respond;
    stringstream read_stream;

    int read_size;
    int package_offset;
    struct Data_Package * package_holder;
    struct Data_Meta * meta_holder;

    deque<struct Data_Package *> read_buffer;
    deque<struct Data_Package *> write_buffer;
    R_Filter * rf;
    W_Filter * wf;

    bool active;

    static function<void (Peer_A *, struct Data_Package *, struct Data_Meta *)> hook_core_recv;

    /////////////////////////////////////
    // STEP 1
    void start();

    // STEP 2
    void verify_client ();

    // STEP 3
    void split_package();

    // STEP 4
    void shutdown();

    /////////////////////////////////////
    Peer_A(ev::default_loop * loop, int socket_id);

    ~Peer_A();

    string info();
};


#endif //PRPR_PEER_A_HPP
