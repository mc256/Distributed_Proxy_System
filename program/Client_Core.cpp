//
// Created by mc on 2/23/18.
//

#include "Client_Core.hpp"

void Client_Core::load_config(){
    ifstream setting_file;
    setting_file.open("client_settings.txt");
    if (setting_file.good()){
        string addr;
        int port;
        string password;
        setting_file >> listen_address >> listen_port >> password_confirm;
        while (setting_file >> addr >> port >> password){
            auto *setting = new Proxy_Peer(addr, port, password);
            interface_b.push_back(setting);
        }
    }else{
        cout << "Cannot load settings" << endl;
        exit(1);
    }
}

void Client_Core::start() {
    // FACE A
    interface_a = new Async_Accept(loop, listen_address, listen_port);
    interface_a->accepted_event = [this](int d, sockaddr_storage r, socklen_t rl){
        int id = connection_a_count ++;
        auto * a = new Client_A(loop, d, id, this);
        connection_a[id] = a;
        a->start();
    };

    // FACE B
    for_each(interface_b.begin(), interface_b.end(), [this](Proxy_Peer *p) {
        p->interface = new Async_Connect(loop, p->address, p->port, DEFAULT_TIMEOUT);
        p->interface->connected_event = [this, p](int d){
            auto * b = new Client_B(loop, p, d, this);
            connection_b.push_back(b);
            b->start();
        };
        p->interface->failed_event = [this](){};
        p->interface->start();
    });
}

Client_Core::Client_Core(ev::default_loop *loop) {
    // Copy
    this->loop = loop;

    // Initialize
    this->connection_a_count = 0;

    // Load Configurations
    load_config();
}

