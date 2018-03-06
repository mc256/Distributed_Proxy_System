//
// Created by mc on 2/23/18.
//

#include "Peer_B.hpp"


void Peer_B::attach_meta(){
    //Adding Package Information
    memcpy(this->meta.sequence_id, (char *) &this->seq_down, sizeof(this->meta.sequence_id));
    memcpy(this->meta.size, (char *) &rf->buffer->back()->size, sizeof(this->meta.size));

    //Copy Meta information to Package
    memcpy(this->read_buffer.back()->buffer, &this->meta, sizeof(this->meta));
    this->read_buffer.back()->size += sizeof(this->meta);

    //Send notification to client core
    if (Peer_B::hook_core_recv != nullptr) {
        Peer_B::hook_core_recv(this, this->read_buffer.back());
    }
    this->seq_down ++;
}

/////////////////////////////////////
void Peer_B::start(){
    this->socket_id = this->peer->descriptor;
    fcntl(this->socket_id, F_SETFL, fcntl(this->socket_id, F_GETFL) | O_NONBLOCK);

    if (this->socket_id == 0){
        cout << "it is zero!!!" << endl;
    }

    // write
    this->wf = new W_Filter(loop, &this->write_buffer, this->socket_id, &this->wf);
    this->wf->set_keep(false);

    //Read
    this->rf = new R_Filter(loop, &this->read_buffer, this->socket_id, &this->rf, this->dispatcher_id);
    this->rf->offset = sizeof(struct Data_Meta);
    this->rf->hook_recv = [this](R_Filter * rf){
        this->attach_meta();
    };

    Peer_B::available_list.push_back(this);
    this->active = true;

    // start everything
    this->rf->start();
    this->wf->start();
}

void Peer_B::flush_sort_pool(){
    while (true){
        auto * d = write_sort_pool[seq_up];
        if (d == nullptr){
            break;
        }
        write_buffer.push_back(d);
        write_sort_pool.erase(seq_up);
        seq_up ++;
    }
    if (this->active && this->rf != NULL && this->wf != NULL){
        this->wf->start();
    }
}

/////////////////////////////////////
Peer_B::Peer_B(ev::default_loop *loop, struct Proxy_Peer * peer, int dispatcher_id) {
    this->loop = loop;
    this->socket_id = peer->descriptor;
    this->dispatcher_id = dispatcher_id;
    this->active = false;

    this->peer = peer;

    Peer_B::interface_list[this->dispatcher_id] = this;
    this->seq_up = 0;
    this->seq_down = 0;
}


Peer_B::~Peer_B() {
    Peer_B::interface_list.erase(this->dispatcher_id);
}

string Peer_B::info() {
    stringstream ss;
    ss << "Peer_B -\t" << socket_id << "\t" << dispatcher_id;
    ss << "\t" << "\tR:" << read_buffer.size() << "\tW:" << write_buffer.size();
    ss << "\t" << (active ? "ON" : "OFF");
    return ss.str();
}

map<int, Peer_B *> Peer_B::interface_list = map<int, Peer_B *>();
vector<Peer_B *> Peer_B::available_list = vector<Peer_B *>();
function<void (Peer_B *, struct Data_Package *)> Peer_B::hook_core_recv = nullptr;