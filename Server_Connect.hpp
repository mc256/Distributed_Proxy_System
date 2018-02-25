//
// Created by mc on 2/23/18.
//

#ifndef PRPR_SERVER_CONNECT_HPP
#define PRPR_SERVER_CONNECT_HPP

#include "main.hpp"


template <class T> class Server_Connect{
private:
    ev::io watcher;
    ev::default_loop * loop;

    struct sockaddr_in connect_address;
    socklen_t connect_address_length;

public:
    struct Proxy_Peer * peer;

    void start();

    Server_Connect(ev::default_loop * loop, struct Proxy_Peer * peer);
};


template<class T>
void Server_Connect<T>::start() {
    CHECK_DO(connect(peer->descriptor, (sockaddr *) &this->connect_address, this->connect_address_length), ,
             "Fail to connect.", delete this;
                     return);
    DEBUG(cout << peer->address << ":" << peer->port << " connected" << endl;)
    peer->connected = true;
    T *t = new T(this->loop, peer);
    t->start();
    delete this;
}

template<class T>
Server_Connect<T>::Server_Connect(ev::default_loop *loop, struct Proxy_Peer *peer) {
    this->loop = loop;
    this->peer = peer;
    this->peer->available = false;
    this->peer->connected = false;

    //IP address
    struct in_addr ipv4;
    inet_pton(AF_INET, peer->address.c_str(), &ipv4);

    //Socket address
    connect_address.sin_family = AF_INET;
    connect_address.sin_port = htons(peer->port);
    connect_address.sin_addr = ipv4;
    connect_address_length = sizeof(struct sockaddr_in);

    //Create Socket
    CHECK(peer->descriptor = socket(AF_INET, SOCK_STREAM, 0), <0, "Cannot create Socket.",);

    thread *t = new thread(&Server_Connect::start, this);
    t->detach();
}


#endif //PRPR_SERVER_CONNECT_HPP
