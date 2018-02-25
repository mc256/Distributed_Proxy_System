//
// Created by mc on 2/23/18.
//

#include "R_Filter.hpp"


void R_Filter::operator()(ev::io &watcher, int revents) {
    struct Data_Package *d = new struct Data_Package;
    d->size = recv(this->socket_id, d->buffer + offset, MAX_BUFFER_SIZE - offset, 0);
    if (d->size > 0) {
        d->sent = 0;
        d->dispatcher_id = this->dispatcher_id;
        d->timestamp = 0;
        buffer->push_back(d);
        if (hook_recv != nullptr) {
            hook_recv(this);
        };
        return;
    } else if (d->size == 0) {
        d->sent = 0;
        this->event_listener.stop();
        d->dispatcher_id = this->dispatcher_id;
        d->timestamp = 0;
        buffer->push_back(d);
        this->active_terminate();
        return;
    } else {
        delete d;
        if (!(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
            this->event_listener.stop();
            this->active_terminate();
        }
    }
}

void R_Filter::start() {
    this->event_listener.start();
}

R_Filter::R_Filter(ev::default_loop *loop, deque<struct Data_Package *> *buffer, int socket_id, R_Filter **back_link,
                   int dispatcher_id) {
    this->loop = loop;
    this->socket_id = socket_id;
    if (buffer == nullptr) {
        this->buffer = new deque<struct Data_Package *>;
    } else {
        this->buffer = buffer;
    }
    this->back_link = back_link;
    this->hook_recv = nullptr;
    this->hook_closed = nullptr;
    this->dispatcher_id = dispatcher_id;
    this->offset = 0;

    this->event_listener.set(*loop);
    this->event_listener.set(this);
    this->event_listener.set(this->socket_id, ev::READ);
}

void R_Filter::active_terminate(){
    this->event_listener.stop();
    shutdown(this->socket_id, SHUT_RDWR);
    close(this->socket_id);

    if (this->back_link != nullptr) {
        *(this->back_link) = nullptr;
    }

    if (hook_closed != nullptr) {
        hook_closed(this);
    }

    delete this;
}

void R_Filter::passive_terminate(){
    this->event_listener.stop();
    shutdown(this->socket_id, SHUT_RDWR);
    close(this->socket_id);

    if (this->back_link != nullptr) {
        *(this->back_link) = nullptr;
    }

    delete this;
}
