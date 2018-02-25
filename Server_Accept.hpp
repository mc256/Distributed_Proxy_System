//
// Created by mc on 2/23/18.
//

#ifndef PRPR_SERVER_ACCEPT_HPP
#define PRPR_SERVER_ACCEPT_HPP

#include "main.hpp"


template <class T> class Server_Accept{
private:
    int socket_id;
    ev::io watcher;
    ev::default_loop * loop;

public:
    void operator()(ev::io &watcher, int revents);

    Server_Accept<T>(ev::default_loop * loop, string address, int port);
};



template<class T>
void Server_Accept<T>::operator()(ev::io &watcher, int revents) {
    //Remote address
    struct sockaddr_storage remote_address;//NOT USED
    socklen_t remote_address_length = sizeof(struct sockaddr_storage);//NOT USED

    int p2c_descriptor;
    CHECK(p2c_descriptor = accept(socket_id, (sockaddr *) &remote_address, &remote_address_length), <0,
          p2c_descriptor << "\tError on accepting connection",);

    T *h = new T(loop, p2c_descriptor);
    thread *t = new thread(&T::start, h);
    t->detach();
}

template<class T>
Server_Accept<T>::Server_Accept(ev::default_loop *loop, string address, int port) {
    //IP address
    struct in_addr ipv4;
    inet_pton(AF_INET, address.c_str(), &ipv4);

    //Socket address
    struct sockaddr_in accept_address;
    accept_address.sin_family = AF_INET;
    accept_address.sin_port = htons(port);
    accept_address.sin_addr = ipv4;
    socklen_t accept_address_length = sizeof(struct sockaddr_in);

    //Create Socket
    CHECK(this->socket_id = socket(AF_INET, SOCK_STREAM, 0), <0, "Cannot create Socket.",);
    CHECK(bind(this->socket_id, (sockaddr *) &accept_address, accept_address_length), ,
          "Cannot bind Socket to address",);
    CHECK(listen(this->socket_id, 0), , "Cannot listen on Socket.",);

    //Bind Listener
    this->loop = loop;
    this->watcher.set(*loop);
    this->watcher.set(this);
    this->watcher.start(this->socket_id, ev::READ);
}

#endif //PRPR_SERVER_ACCEPT_HPP
