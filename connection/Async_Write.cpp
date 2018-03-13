//
// Created by mc on 3/7/18.
//

#include "Async_Write.hpp"

void Async_Write::stop_watchers() {
    this->timeout_watcher.stop();
    this->write_io_watcher.stop();
}

void Async_Write::write_callback(ev::io &w, int r) {
    auto s = send(this->descriptor, this->buffer + this->position, this->length - this->position, 0);
    if (s > 0){
        this->position += s;
        if (this->position == this->length){
            this->stop_watchers();
            this->wrote_event(this->buffer, this->position);
        }
    }else if( s == 0){
        this->stop_watchers();
        this->closed_event(this->buffer, this->position);
    }else{
        if (!(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
            this->stop_watchers();
            this->failed_event(this->buffer, this->position);
        }
    }
}

void Async_Write::timeout_callback(ev::timer &w, int r) {
    this->stop_watchers();
    this->failed_event(this->buffer, this->length);
}

void Async_Write::set_timeout(int i) {
    this->timeout = i;
}

void Async_Write::start() {
    if (timeout != 0){
        this->timeout_watcher.start(timeout, timeout);
    }
    this->write_io_watcher.start();
}


void Async_Write::reset(){
    this->position = 0;
}

void Async_Write::reset(char *buffer, ssize_t length){
    this->buffer = buffer;
    this->length = length;
    this->position = 0;
}

Async_Write::Async_Write(ev::default_loop *loop, int descriptor) {
    // Copy values
    this->descriptor = descriptor;
    this->position = 0;
    this->timeout = 0;

    // Set watchers
    this->write_io_watcher.set(*loop);
    this->write_io_watcher.set<Async_Write, &Async_Write::write_callback>(this);
    this->write_io_watcher.set(this->descriptor, ev::WRITE);

    this->timeout_watcher.set(*loop);
    this->timeout_watcher.set<Async_Write, &Async_Write::timeout_callback>(this);

}

Async_Write::Async_Write(ev::default_loop *loop, int descriptor, char *buffer, ssize_t length) {
    // Copy values
    this->descriptor = descriptor;
    this->buffer = buffer;
    this->position = 0;
    this->length = length;
    this->timeout = 0;

    // Set watchers
    this->write_io_watcher.set(*loop);
    this->write_io_watcher.set<Async_Write, &Async_Write::write_callback>(this);
    this->write_io_watcher.set(this->descriptor, ev::WRITE);

    this->timeout_watcher.set(*loop);
    this->timeout_watcher.set<Async_Write, &Async_Write::timeout_callback>(this);
}

Async_Write::~Async_Write() {
    this->stop_watchers();
    this->wrote_event = nullptr;
    this->closed_event = nullptr;
    this->failed_event = nullptr;
}