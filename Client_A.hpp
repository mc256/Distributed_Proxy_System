//
// Created by mc on 2/23/18.
//

#ifndef PRPR_CLIENT_A_HPP
#define PRPR_CLIENT_A_HPP

#include "main.hpp"

class Client_A{
private:
    ev::default_loop * loop;
    int socket_id;
    int dispatcher_id;
    static int unique_id;
    int seq_up, seq_down;

    struct Data_Meta meta;


    void attach_meta();
public:

    static map<int, Client_A *> interface_list;

    deque<struct Data_Package *> read_buffer;
    deque<struct Data_Package *> write_buffer;
    map<int,struct Data_Package *> write_sort_pool;

    R_Filter * rf;
    W_Filter * wf;

    bool active;

    static function<void (Client_A *)> hook_core_recv;

    void start();
    void flush_sort_pool();

    /////////////////////////////////////
    Client_A(ev::default_loop * loop, int socket_id);
    ~Client_A();

    string info();
};



#endif //PRPR_CLIENT_A_HPP
