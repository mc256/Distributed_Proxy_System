//
// Created by mc on 2/23/18.
//

#ifndef PRPR_CLIENT_CORE_HPP
#define PRPR_CLIENT_CORE_HPP

#include "main.hpp"

class Client_Core{
private:
    // Face A
    Server_Accept<Client_A> * sa_ca;

    // Face B
    vector<struct Proxy_Peer *> pp_list;

    // System
    ev::default_loop * loop;
    string address;
    int port;

    // Configure
    int instance_id = 678;

    void up_link_transport(Client_A * a);
    void down_link_transport(Client_B * b, Data_Package * d, Data_Meta * m);
    void load_config();

public:
    static string password_confirm;

    /////////////////////////////////////
    Client_Core(ev::default_loop * loop, string address, int port);

};


#endif //PRPR_CLIENT_CORE_HPP
