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

            // Client_A should hold the origin data package, therefore we need a copy
            auto * nd = new struct Data_Package;
            memcpy(nd,d,sizeof(struct Data_Package));

            // Push to send queue
            b->write_buffer.push_back(nd);
        }
    });
    b->wf->start();
}


void Client_Core::load_config(){
    ifstream setting_file;
    setting_file.open("client_settings.txt");
    if (setting_file.good()){
        string addr;
        int port;
        string password;
        setting_file >> password_confirm;
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


Client_Core::Client_Core(ev::default_loop *loop, string address, int port) {
    this->loop = loop;
    this->address = address;
    this->port = port;

    load_config();

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

string Client_Core::password_confirm = "";