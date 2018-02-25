//
// Created by mc on 2/23/18.
//

#include "W_Filter.hpp"

void W_Filter::operator()(ev::io &watcher, int revents) {
    if (this->current == nullptr) {
        if (buffer->size() > 0) {
            this->current = buffer->front();
            buffer->pop_front();
        } else {
            this->event_listener.stop();
            if (this->hook_finished != nullptr) {
                this->hook_finished(this);
            }
            return;
        }
    }
    int t = send(this->socket_id, this->current->buffer + this->current->sent + this->offset,
                 this->current->size - this->current->sent - this->offset, 0);
    if (t > 0) {
        this->current->sent += t;
        if (this->current->sent == this->current->size) {
            if (!this->keep) {
                delete this->current;
            }
            this->current = nullptr;
            if (this->hook_sent != nullptr) {
                this->hook_sent(this);
            }
        }
    } else if (t == 0) {
        // this is an unsuccessful case, restore the data.
        // https://stackoverflow.com/questions/3081952/with-c-tcp-sockets-can-send-return-zero
        this->current->sent = 0;
        buffer->push_front(this->current);
        this->event_listener.stop();
        this->active_terminate();
        return;
    } else {
        if (!(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
            this->current->sent = 0;
            buffer->push_front(this->current);
            this->event_listener.stop();
            this->active_terminate();
        }
    }
}

void W_Filter::start() {
    this->event_listener.start();
}

void W_Filter::set_keep(bool b) {
    this->keep = b;
}

W_Filter::W_Filter(ev::default_loop *loop, deque<struct Data_Package *> *buffer, int socket_id, W_Filter **back_link) {
    this->loop = loop;
    this->socket_id = socket_id;
    this->buffer = buffer;
    this->back_link = back_link;
    this->hook_sent = nullptr;
    this->hook_finished = nullptr;
    this->hook_closed = nullptr;
    this->current = nullptr;
    this->offset = 0;

    this->event_listener.set(*loop);
    this->event_listener.set(this);
    this->event_listener.set(this->socket_id, ev::WRITE);
}

void W_Filter::active_terminate() {
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

void W_Filter::passive_terminate() {
    this->event_listener.stop();
    shutdown(this->socket_id, SHUT_RDWR);
    close(this->socket_id);

    if (this->back_link != nullptr) {
        *(this->back_link) = nullptr;
    }

    delete this;
}
