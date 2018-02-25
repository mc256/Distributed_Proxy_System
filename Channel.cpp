//
// Created by mc on 2/23/18.
//

#include "Channel.hpp"


void Channel::terminate() {
    delete this;
}

void Channel::passive_terminate() {
    this->read_io_watcher.stop();
    this->write_io_watcher.stop();

    shutdown(this->read_descriptor, SHUT_RDWR);
    shutdown(this->write_descriptor, SHUT_RDWR);

    close(this->read_descriptor);
    close(this->write_descriptor);

    this->terminate();
}

void Channel::active_terminate() {
    this->read_io_watcher.stop();
    this->write_io_watcher.stop();

    this->pair->passive_terminate();

    shutdown(this->read_descriptor, SHUT_RDWR);
    shutdown(this->write_descriptor, SHUT_RDWR);

    close(this->read_descriptor);
    close(this->write_descriptor);

    this->terminate();
}


void Channel::recv_callback(ev::io &watcher, int revents) {
    int t = recv(this->read_descriptor, &(this->buffer), MAX_BUFFER_SIZE, 0);

    if (t > 0) {
        this->buffer_length = t;
        this->buffer_sent = 0;

        this->read_io_watcher.stop();
        this->write_io_watcher.start();
    } else if (t == 0) {
        this->read_io_watcher.stop();
        this->active_terminate();
    } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            return;
        } else {
            this->read_io_watcher.stop();
            this->active_terminate();
        }
    }
}

void Channel::send_callback(ev::io &watcher, int revents) {
    int t = send(this->write_descriptor, &(this->buffer[buffer_sent]), this->buffer_length - this->buffer_sent, 0);

    if (t > 0) {
        this->buffer_sent += t;
        if (this->buffer_sent == this->buffer_length) {
            this->write_io_watcher.stop();
            this->read_io_watcher.start();
        }
    } else if (t == 0) {
        this->write_io_watcher.stop();
        this->active_terminate();
    } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            return;
        } else {
            this->write_io_watcher.stop();
            this->active_terminate();
        }
    }
}

void Channel::setPair(Channel *anotherChannel) {
    this->pair = anotherChannel;
}

void Channel::start() {
    this->read_io_watcher.start();
}

Channel::Channel(ev::default_loop *loop, int read, int write) {
    this->loop = loop;
    this->read_descriptor = read;
    this->write_descriptor = write;

    this->read_io_watcher.set(*loop);
    this->read_io_watcher.set<Channel, &Channel::recv_callback>(this);
    this->read_io_watcher.set(this->read_descriptor, ev::READ);

    this->write_io_watcher.set(*loop);
    this->write_io_watcher.set<Channel, &Channel::send_callback>(this);
    this->write_io_watcher.set(this->write_descriptor, ev::WRITE);
}
