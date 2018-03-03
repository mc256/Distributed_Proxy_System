//
// Created by mc on 2/23/18.
//

#include "Client_Core.hpp"

void Client_Core::up_link_transport(Client_A * a){
    // copy to send queue on B
    // randomly select one peer from available proxy peer list
    Client_B * b = Client_B::available_list[rand() % Client_B::available_list.size()];
    for_each(a->read_buffer.begin(),a->read_buffer.end(), [this, b](struct Data_Package * d){
        // If it has wait for too long, resend
        if (d->timestamp < time(NULL) - RESEND_PERIOD){
            // Update Timestamp
            d->timestamp = time(NULL);

            // Client_B should hold the origin data package, therefore we need a copy
            auto * nd = new struct Data_Package;
            memcpy(nd,d,sizeof(struct Data_Package));

            // Push to send queue
            b->write_buffer.push_back(nd);
        }
    });
    b->wf->start();
}

Client_Core::Client_Core(ev::default_loop *loop, string address, int port) {
    this->loop = loop;
    this->address = address;
    this->port = port;

    // load peer settings
    struct Proxy_Peer *pp = new struct Proxy_Peer;
    pp->address = "127.0.0.1";
    pp->port = 9101;
    pp->password = "password";
    pp_list.push_back(pp);
    pp = new struct Proxy_Peer;
    pp->address = "127.0.0.1";
    pp->port = 9102;
    pp->password = "password";
    pp_list.push_back(pp);
    pp = new struct Proxy_Peer;
    pp->address = "127.0.0.1";
    pp->port = 9103;
    pp->password = "password";
    pp_list.push_back(pp);
    pp = new struct Proxy_Peer;
    pp->address = "127.0.0.1";
    pp->port = 9104;
    pp->password = "password";
    pp_list.push_back(pp);


    // A face ------------------------------------------------------------
    Client_A::hook_core_recv = [this](Client_A * a){
        this->up_link_transport(a);
    };
    this->sa_ca = new Server_Accept<Client_A>(loop, address, port);

    // B face ------------------------------------------------------------
    Client_B::hook_core_recv = [this](Client_B *b, struct Data_Package * d) {
        // copy to write queue on A

    };
    for_each(pp_list.begin(), pp_list.end(), [this](struct Proxy_Peer *p) {
        (new Server_Connect<Client_B>(new Client_B(this->loop, p)));
    });

}