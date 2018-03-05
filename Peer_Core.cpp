//
// Created by mc on 2/25/18.
//

#include "Peer_Core.hpp"

void Peer_Core::up_link_transport(Peer_A * a, struct Data_Package * d, struct Data_Meta * m){
    // Retire package information
    auto dispatcher_id = (int) *m->dispatcher_id;
    auto sequence_id = (int) *m->sequence_id;
    auto user_id = (int) *m->user_id;

    // dispatch to specific Peer_B classes
    auto * interface = &Peer_B::interface_list[dispatcher_id];
    if (* interface == nullptr){
        new Peer_B(this->loop, this->pp_list[rand()%this->pp_list.size()], dispatcher_id);
        new Server_Connect<Peer_B>(* interface);
    }
    (*interface)->write_sort_pool[sequence_id] = d;
    (*interface)->flush_sort_pool();
}

void Peer_Core::down_link_transport(Peer_B * b, struct Data_Package * d){
    // copy to send queue on A
    // randomly select one peer from available proxy peer list
    Peer_A * a = Peer_A::available_list[rand() % Peer_A::available_list.size()];
    for_each(b->read_buffer.begin(), b->read_buffer.end(), [this, a](struct Data_Package * d){
        // If it has waited for too long, resend the package
        if (d->timestamp < time(NULL) - RESEND_PERIOD) {
            // Update Timestap
            d->timestamp = time(NULL);

            // Peer_B should hold the origin data package, therefore we need a copy
            auto * nd = new struct Data_Package;
            memcpy(nd, d, sizeof(struct Data_Package));

            // Push to write send queue
            a->write_buffer.push_back(nd);
        }
    });
}

void Peer_Core::load_config(){
    ifstream setting_file;
    setting_file.open("peer_settings.txt");
    if (setting_file.good()){
        string addr;
        int port;
        string password;
        setting_file >> password_connect >> password_confirm;
        while (setting_file >> addr >> port >> password){
            struct Proxy_Peer *setting = new struct Proxy_Peer;
            setting->address = addr;
            setting->port = port;
            setting->password = password;
            pp_list.push_back(setting);
        }
    }else{
        cout << "Cannot load settings" << endl;
        exit(1);
    }
}

Peer_Core::Peer_Core(ev::default_loop *loop, string address, int port_begin, int port_end) {
    this->loop = loop;

    load_config();

    // A face ------------------------------------------------------------
    Peer_A::hook_core_recv = [this](Peer_A * a, struct Data_Package * d, struct Data_Meta * m){
        this->up_link_transport(a, d, m);
    };
    for (int port = port_begin; port <= port_end; ++port) {
        (new Server_Accept<Peer_A>(loop, "0.0.0.0", port));
    }


    // B face ------------------------------------------------------------
    Peer_B::hook_core_recv = [this](Peer_B * b, struct Data_Package * d){
        this->down_link_transport(b, d);
    };

}

string Peer_Core::password_connect = "";
string Peer_Core::password_confirm = "";