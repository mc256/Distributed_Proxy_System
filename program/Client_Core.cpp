//
// Created by mc on 2/23/18.
//

#include "Client_Core.hpp"

void Client_Core::load_config(){
    ifstream setting_file;
    setting_file.open("client_settings.txt");
    if (setting_file.good()){
        // Basic config
        setting_file >> listen_address >> listen_port >> password_confirm;

        // Port address
        string address;
        int port;
        string password;
        while (setting_file >> address >> port >> password){
            auto *setting = new Proxy_Peer(address, port, password);
            interface_b.push_back(setting);
        }
    }else{
        cout << "Cannot load settings" << endl;
        exit(1);
    }
}

void Client_Core::reconnect() {
    for_each(interface_b.begin(), interface_b.end(), [this](Proxy_Peer *p) {
        p->interface_connect = new Async_Connect(loop, p->address, p->port, DEFAULT_TIMEOUT);
        p->interface_connect->connected_event = [this, p](int d){
            auto * b = new Client_B(loop, p, d, this);
            connection_b.push_back(b);
            b->start();
        };
        p->interface_connect->failed_event = [this](){};
        p->interface_connect->start();
    });
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
    interface_a->start();

    // FACE B
    // reconnect();

    // Scheduler
    watcher.start();

}


void Client_Core::operator()(ev::timer &w, int r) {
    // Reconnect Peers
    if (connection_b_available.size() < LOWEST_CONNECTION){
        reconnect();
    }

    // Retransmit Package
    for (const auto &kv : connection_a) {
        if (kv.second == nullptr)continue;
        kv.second->up_link_transmit();
        kv.second->start_writer();
    }
}

Client_Core::Client_Core(ev::default_loop *loop) {
    // Copy
    this->loop = loop;

    // Initialize
    this->connection_a_count = 0;

    this->watcher.set(*loop);
    this->watcher.set(this);
    this->watcher.start(0, Encryption::get_random(RESEND_PERIOD + 1, RESEND_PERIOD));

    // Load Configurations
    load_config();
}

