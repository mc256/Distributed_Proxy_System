//
// Created by mc on 3/7/18.
//

#ifndef PRPR_CONTAINER_HPP
#define PRPR_CONTAINER_HPP

#include "../main.hpp"

class Container {
public:
    string mode;

    Client_Core * cc;
    Peer_Core * pc;
    Socks_Core * sc;


    ev::default_loop * loop;



    Container();

};


#endif //PRPR_CONTAINER_HPP
