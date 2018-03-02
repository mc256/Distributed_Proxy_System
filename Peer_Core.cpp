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
        auto * dispatcher_id = (int *) m.dispatcher_id;
        auto * sequence_id = (int *) m.sequence_id;

        // dispatch to specific Peer_B classes
        auto * interface = &Peer_B::interface_list[*dispatcher_id];
        if (* interface == nullptr){
            * interface = new Peer_B(this->loop, this->pp_list[rand()%this->pp_list.size()], *dispatcher_id);
            new Server_Connect<Peer_B>(* interface);
        }
        (*interface)->write_buffer.push_back(d);
        (*interface)->wf->start();
    };
    this->sa_pa = new Server_Accept<Peer_A>(loop, address, port);

    // B face ------------------------------------------------------------
    Peer_B::hook_core_recv = [this](Peer_B * b, struct Data_Package * d){
        struct Data_Package * nd = new struct Data_Package;
        memcpy(&nd, &d, sizeof(struct Data_Meta));
        Peer_A * a = Peer_A::available_list[rand() % Peer_A::available_list.size()];
        for_each(b->read_buffer.begin(), b->read_buffer.end(), [this, b](struct Data_Package * d){
            if (d->timestamp < time(NULL) - RESEND_PERIOD) {
                // TODO:
            }
        });
    };
}