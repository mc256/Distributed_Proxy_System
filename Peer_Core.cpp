//
// Created by mc on 2/25/18.
//

#include "Peer_Core.hpp"


Peer_Core::Peer_Core(ev::default_loop *loop) {

    // load peer settings
    struct Proxy_Peer *pp = new struct Proxy_Peer;
    pp->address = "127.0.0.1";
    pp->port = 9111;
    pp->password = "password";
    pp_list.push_back(pp);
    pp = new struct Proxy_Peer;
    pp->address = "127.0.0.1";
    pp->port = 9112;
    pp->password = "password";
    pp_list.push_back(pp);
    pp = new struct Proxy_Peer;
    pp->address = "127.0.0.1";
    pp->port = 9113;
    pp->password = "password";
    pp_list.push_back(pp);
    pp = new struct Proxy_Peer;
    pp->address = "127.0.0.1";
    pp->port = 9114;
    pp->password = "password";
    pp_list.push_back(pp);


    // A face ------------------------------------------------------------
    Peer_A::hook_core_recv = [this](Peer_A * a, struct Data_Package * d){
        // Retire package information
        struct Data_Meta m;
        memcpy(&m, d->buffer, sizeof(struct Data_Meta));
        m.
        // dispatch to specific Peer_B classes
    };
    this->sa_pa = new Server_Accept<Peer_A>(loop, address, port);

    // B face ------------------------------------------------------------
}