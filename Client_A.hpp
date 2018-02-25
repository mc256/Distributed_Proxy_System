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
    static int unique_id;
    int sequence_id;
    struct Data_Meta meta;
    bool actived;
public:
    int dispatcher_id;

    static map<int, Client_A *> interface_list;

    deque<struct Data_Package *> read_buffer;
    deque<struct Data_Package *> write_buffer;
    R_Filter * rf;
    W_Filter * wf;

    static function<void (Client_A *)> hook_core_recv;

    void start();

    /////////////////////////////////////
    Client_A(ev::default_loop * loop, int socket_id);
    ~Client_A();

    string info();
};



#endif //PRPR_CLIENT_A_HPP
