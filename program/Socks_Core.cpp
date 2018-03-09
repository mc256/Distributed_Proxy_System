//
// Created by mc on 3/8/18.
//

#include "Socks_Core.hpp"

void Socks_Core::load_config() {
    ifstream setting_file;
    setting_file.open("socks_settings.txt");
    if (setting_file.good()){
        setting_file >> listen_address >> listen_port ;
    }else{
        cout << "Cannot load settings" << endl;
        exit(1);
    }

}

void Socks_Core::start() {
    proxy_server = new Async_Accept(loop, listen_address, listen_port);
    proxy_server->accepted_event = [this](int d, sockaddr_storage r, socklen_t rl){
        fcntl(d, F_SETFL, fcntl(d, F_GETFL) & (~O_NONBLOCK));
        Handshake * h = new Handshake(loop, d);
        thread * t = new thread(&Handshake::start, h);
        t->detach();
    };
    proxy_server->start();
}

Socks_Core::Socks_Core(ev::default_loop *loop) {
    this->loop = loop;
    load_config();
}