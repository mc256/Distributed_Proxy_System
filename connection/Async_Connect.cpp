//
// Created by mc on 3/6/18.
//

#include "Async_Connect.hpp"

void Async_Connect::stop_watchers() {
    this->read_io_watcher.stop();
    this->write_io_watcher.stop();
    this->timeout_watcher.stop();
}

void Async_Connect::read_callback(ev::io &w, int r) {
    this->stop_watchers();
    this->connected_event(this->descriptor);
}

void Async_Connect::write_callback(ev::io &w, int r) {
    this->stop_watchers();
    this->connected_event(this->descriptor);
}

void Async_Connect::timeout_callback(ev::timer &w, int r) {
    this->stop_watchers();
    this->failed_event();
}

void Async_Connect::start(){
    //Start Watcher
    this->read_io_watcher.start();
    this->write_io_watcher.start();
    this->timeout_watcher.start();

    //Connect
    connect(this->descriptor, (sockaddr *) &this->connect_address, this->connect_address_length);
}

Async_Connect::Async_Connect(ev::default_loop *loop,
                             string address, int port, int timeout) {

    //IP address
    in_addr ipv4 = in_addr();
    inet_pton(AF_INET, address.c_str(), &ipv4);

    //Socket address
    connect_address.sin_family = AF_INET;
    connect_address.sin_port = htons((uint16_t) port);
    connect_address.sin_addr = ipv4;
    connect_address_length = sizeof(struct sockaddr_in);

    //Create Socket
    CHECK(this->descriptor = socket(AF_INET, SOCK_STREAM, 0), <0, "Cannot create Socket.",);

    //Set unblock
    fcntl(this->descriptor, F_SETFL, fcntl(this->descriptor, F_GETFL) | O_NONBLOCK);

    //Set watcher
    this->read_io_watcher.set(*loop);
    this->read_io_watcher.set<Async_Connect, &Async_Connect::read_callback>(this);
    this->read_io_watcher.set(this->descriptor, ev::READ);


    this->write_io_watcher.set(*loop);
    this->write_io_watcher.set<Async_Connect, &Async_Connect::write_callback>(this);
    this->write_io_watcher.set(this->descriptor, ev::WRITE);


    this->timeout_watcher.set(*loop);
    this->timeout_watcher.set<Async_Connect, &Async_Connect::timeout_callback>(this);
    this->timeout_watcher.set(timeout, timeout);
}