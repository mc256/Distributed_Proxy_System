//
// Created by mc on 2/23/18.
//

#ifndef PRPR_CLIENT_B_HPP
#define PRPR_CLIENT_B_HPP

#include "main.hpp"

class Client_B{

private:
    ev::default_loop * loop;
    int socket_id;
    int dispatcher_id;
    static int unique_id;

    void generate_fake_request();

public:
    static map<int, Client_B *> interface_list;
    static vector<Client_B *> available_list;

    struct Proxy_Peer * peer;

    stringstream read_stream;

    int read_size;
    int package_offset;
    struct Data_Package * package_holder;
    struct Data_Meta * meta_holder;

    deque<struct Data_Package *> read_buffer;
    deque<struct Data_Package *> write_buffer;
    W_Filter * wf;
    R_Filter * rf;

    bool active;

    static function<void (Client_B *, struct Data_Package *, Data_Meta *)> hook_core_recv;

    void remove_package_with_dispatcher_id(int id);

    /////////////////////////////////////
    // STEP 1
    void start();

    // STEP 2
    void verify_peer();

    // STEP 3
    void split_package();

    // STEP 4
    void shutdown();

    /////////////////////////////////////
    Client_B(ev::default_loop * loop, struct Proxy_Peer * peer);

    ~Client_B();

    string info();

};


#endif //PRPR_CLIENT_B_HPP
