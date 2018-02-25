//
// Created by mc on 2/23/18.
//

#include "Peer_A.hpp"


void Peer_A::flush_buffer() {
    while (!this->read_buffer.empty()) {
        struct Data_Package *data = this->read_buffer.front();
        read_ss.write((char *) data->buffer, (size_t) data->size);
        this->read_buffer.pop_front();
    }
}


/////////////////////////////////////
// STEP 1
void Peer_A::start() {
    fcntl(this->socket_id, F_SETFL, fcntl(this->socket_id, F_GETFL) | O_NONBLOCK);

    // write
    this->wf = new W_Filter(loop, &this->write_buffer, this->socket_id, &this->wf);
    this->wf->set_keep(false);

    //Read
    this->rf = new R_Filter(loop, &this->read_buffer, this->socket_id, &this->rf, this->dispatcher_id);
    this->rf->hook_recv = [this](R_Filter *rf) {
        this->verify_client();
    };

    // start everything
    this->rf->start();
    this->wf->start();
}

// STEP 2
void Peer_A::verify_client() {

    this->flush_buffer();
    DEBUG(cout << read_ss.str() << endl;);

    //Prepare Web Page
    static string http_response = "HTTP/1.1 200 OK\r\nServer: nginx\r\nContent-Type: text/html\r\nConnection: keep-alive\r\nLocation: https://errno104.com/\r\n\r\n<html>\r\n<head><title>301 Moved Permanently</title></head><body><center><h1>301 Moved Permanently</h1></center><hr><center>nginx</center></body></html>\r\n";
    string key = Encryption::sha_hash("TODO");
    string response = http_response + key;
    DEBUG(cout << "fake http response:" << endl << response << endl;)

    //Prepare package
    struct Data_Package *data = new struct Data_Package;
    data->size = response.length();
    data->sent = 0;
    for (int i = 0; i < response.length(); i++) { data->buffer[i] = response.at(i); }
    this->write_buffer.push_back(data);
    this->wf = new W_Filter(this->loop, &this->write_buffer, this->socket_id, &this->wf);
    this->wf->set_keep(false);
    //Get ready for real job
    this->wf->hook_finished = [this](W_Filter *wf) {
        this->rf->hook_recv = nullptr;
        this->active = true;
        this->wf->set_keep(true);
    };
    this->wf->start();
}

/////////////////////////////////////
Peer_A::Peer_A(ev::default_loop *loop, int socket_id) {
    this->loop = loop;
    this->socket_id = socket_id;
    this->active = false;

    this->dispatcher_id = ++unique_id;
    Peer_A::interface_list[this->dispatcher_id] = this;
}

Peer_A::~Peer_A() {
    Peer_A::interface_list.erase(this->dispatcher_id);
}

string Peer_A::info() {
    stringstream ss;
    ss << "Peer_A -\t" << socket_id << "\t" << dispatcher_id;
    ss << "\t" << "\tR:" << read_buffer.size() << "\tW:" << write_buffer.size();
    ss << "\t" << (active ? "ON" : "OFF");
    return ss.str();
}


int Peer_A::unique_id = 0;
map<int, Peer_A *> Peer_A::interface_list = map<int, Peer_A *>();
function<void (Peer_A *, struct Data_Package *)> Peer_A::hook_core_recv = nullptr;