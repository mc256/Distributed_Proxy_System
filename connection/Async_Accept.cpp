//
// Created by mc on 3/7/18.
//

#include "Async_Accept.hpp"


void Async_Accept::read_callback(ev::io &w, int r) {
    //Remote address
    sockaddr_storage remote_address = sockaddr_storage();
    socklen_t remote_address_length = sizeof(sockaddr_storage);

    //Accept
    int accepted_descriptor;
    CHECK(accepted_descriptor = accept(this->descriptor, (sockaddr *) &remote_address, &remote_address_length), <0,
          accepted_descriptor << "\tError on accepting connection",);

    //Set unblock
    fcntl(accepted_descriptor, F_SETFL, fcntl(accepted_descriptor, F_GETFL) | O_NONBLOCK);

    //Pass back
    this->accepted_event(accepted_descriptor, remote_address, remote_address_length);
}


void Async_Accept::start() {
    this->read_io_watcher.start();
}

Async_Accept::Async_Accept(ev::default_loop *loop, string address, int port) {

    //IP address
    in_addr ipv4 = in_addr();
    inet_pton(AF_INET, address.c_str(), &ipv4);

    //Socket address
    accept_address.sin_family = AF_INET;
    accept_address.sin_port = htons((uint16_t) port);
    accept_address.sin_addr = ipv4;
    accept_address_length = sizeof(sockaddr_in);

    //Create Socket
    CHECK(this->descriptor = socket(AF_INET, SOCK_STREAM, 0), <0, "Cannot create Socket.",);
    CHECK(
            bind(this->descriptor, (sockaddr *) &accept_address, accept_address_length), ,
            "Cannot bind Socket to address",
    );
    CHECK(listen(this->descriptor, 0), , "Cannot listen on Socket.",);

    //Set watcher
    this->read_io_watcher.set(*loop);
    this->read_io_watcher.set<Async_Accept, &Async_Accept::read_callback>(this);
    this->read_io_watcher.set(this->descriptor, ev::READ);

}

Async_Accept::~Async_Accept() {
    this->read_io_watcher.stop();
    this->accepted_event = nullptr;
}