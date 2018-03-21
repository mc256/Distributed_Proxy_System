//
// Created by mc on 3/9/18.
//

#include "Peer_Core.hpp"

void Peer_Core::load_config() {
    ifstream setting_file;
    setting_file.open("peer_settings.txt");
    if (setting_file.good()) {
        setting_file >> listen_address >> listen_port_begin >> listen_port_end;
        setting_file >> password >> confirm_password;
        setting_file >> socks_address >> socks_port;
        setting_file >> fake_source;
    } else {
        cout << "Cannot load settings" << endl;
        exit(1);
    }
}

void Peer_Core::start() {
    // FACE A
    for (int i = listen_port_begin; i <= listen_port_end; ++i) {
        auto *interface = new Async_Accept(loop, listen_address, i);
        interface->accepted_event = [this](int d, sockaddr_storage r, socklen_t rl) {
            auto *a = new Peer_A(loop, d, this);
            connection_a.push_back(a);
            a->start();
        };
        interface->start();
    }

    // FACE B
    // Do nothing, the FACE B will create new connection if needed


    // Scheduler
    watcher.start();
}

Peer_B *Peer_Core::connect_socks_server(int dispatcher) {

    auto *b = new Peer_B(loop, dispatcher, this);
    connection_b[dispatcher] = b;

    auto *connection = new Async_Connect(loop, socks_address, socks_port, DEFAULT_TIMEOUT);
    connection->connected_event = [this, b](int d) {
        b->socket_id = d;
        if (!b->flag_terminated){
            b->start();
            b->start_writer();
        }else{
            close(d);
            delete b;
        }
    };
    connection->failed_event = [this, b]() {
        delete b;
    };
    connection->start();

    return b;

}


void Peer_Core::operator()(ev::timer &w, int r) {
    // Retransmit Package
    for (const auto &kv : connection_b) {
        if (kv.second == nullptr)continue;
        kv.second->down_link_transmit();
        kv.second->start_writer();
    }
}

Peer_Core::Peer_Core(ev::default_loop *loop) {
    // Copy
    this->loop = loop;

    // Initialize
    this->watcher.set(*loop);
    this->watcher.set(this);
    this->watcher.start(0, Encryption::get_random(RESEND_PERIOD + 1, RESEND_PERIOD));

    // Load Configurations
    load_config();
}