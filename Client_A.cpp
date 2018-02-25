//
// Created by mc on 2/23/18.
//

#include "Client_A.hpp"


void Client_A::start() {
    fcntl(this->socket_id, F_SETFL, fcntl(this->socket_id, F_GETFL) | O_NONBLOCK);
    this->actived = true;

    // Read
    this->rf = new R_Filter(this->loop, &this->read_buffer, this->socket_id, &this->rf, this->dispatcher_id);
    this->rf->offset = sizeof(struct Data_Meta);
    this->rf->hook_recv = [this](R_Filter *rf) {
        //Adding Package Information
        memcpy(this->meta.sequence_id, (char *) &this->sequence_id, sizeof(this->meta.sequence_id));
        memcpy(this->meta.size, (char *) &rf->buffer->back()->size, sizeof(this->meta.size));
        memcpy(this->read_buffer.back()->buffer, &this->meta, sizeof(this->meta));
        //Send notification to client core
        if (Client_A::hook_core_recv != nullptr) {
            Client_A::hook_core_recv(this);
        }
        this->sequence_id ++;
    };
    this->rf->hook_closed = [this](R_Filter *rf) {
        actived = false;
    };

    // Write
    this->wf = new W_Filter(this->loop, &this->write_buffer, this->socket_id, &this->wf);
    this->wf->offset = sizeof(struct Data_Meta);
    this->wf->hook_closed = [this](W_Filter *wf) {
        actived = false;
    };
    this->wf->set_keep(false);

    // Start Everything
    this->rf->start();
    this->wf->start();
}


/////////////////////////////////////
Client_A::Client_A(ev::default_loop *loop, int socket_id) {
    this->loop = loop;
    this->socket_id = socket_id;
    this->sequence_id = 0;

    this->dispatcher_id = ++unique_id;
    Client_A::interface_list[this->dispatcher_id] = this;

    memcpy(this->meta.dispatcher_id, (char *) &this->dispatcher_id, sizeof(this->dispatcher_id));
}

Client_A::~Client_A() {
    if (this->rf != nullptr) {
        this->rf->passive_terminate();
    }
    if (this->wf != nullptr) {
        this->wf->passive_terminate();
    }
    for (int i = 0; i < read_buffer.size(); i++) {
        delete read_buffer[i];
    }
    read_buffer.clear();
    for (int i = 0; i < write_buffer.size(); i++) {
        delete write_buffer[i];
    }
    write_buffer.clear();
    Client_A::interface_list.erase(this->dispatcher_id);
}


string Client_A::info() {
    stringstream ss;
    ss << "Client_A -\t socket ID:" << socket_id << "\t\tR:" << this->read_buffer.size() << " W:"
       << this->write_buffer.size() << "\t"
       << (this->actived ? "ON" : "OFF");
    return ss.str();
}


int Client_A::unique_id = 0;
map<int, Client_A *> Client_A::interface_list = map<int, Client_A *>();
function<void(Client_A *)> Client_A::hook_core_recv = nullptr;