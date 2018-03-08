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


Client_Core::Client_Core(ev::default_loop *loop) {
    // Copy
    this->loop = loop;

    // Initialize
    this->connection_a_count = 0;
    this->connection_b_count = 0;

    // Load Configurations
    load_config();

    // FACE A
    this->interface_a = new Async_Accept(loop, this->listen_address, this->listen_port);
    this->interface_a->accepted_event = [this](int descriptor, sockaddr_storage remote_address, socklen_t remote_address_length){
        int id = this->connection_a_count ++;
        this->connection_a[id] = new Client_A(this->loop, descriptor, id, this);
    };

    // FACE B
    for_each(interface_b.begin(), interface_b.end(), [this](Proxy_Peer *p) {
        p->interface = new Async_Connect(this->loop, p->address, p->port, DEFAULT_TIMEOUT);

        // Connect
        p->interface->connected_event = [this](int d){
            int id = this->connection_b_count ++;
            this->connection_b.push_back(new Client_B(this->loop, p, id, this));
        };
        p->interface->failed_event = [this](){};
        p->interface->start();
    });
}

