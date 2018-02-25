//
// Created by mc on 2/23/18.
//

#include "Peer_A.hpp"

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
    struct Data_Package * data = this->read_buffer.front();
    //DEBUG(cout << string((char *) data->buffer) << endl;)

    handshake_str << string((char *) data->buffer);
    size_t found = handshake_str.str().find("\n");
    if (found == string::npos){
        return;
    }

    stringstream response;
    if (!fake_header_respond){
        string method, uri, protocol;
        handshake_str >> method >> uri >> protocol;
        if (protocol == "HTTP/1.1" && method == "GET"){
            response << "HTTP/1.1 200 OK\r\nServer: nginx\r\nContent-Type: text/html\r\nLocation: https://errno104.com/\r\n\r\n<html>\r\n<head><title>Hello World!</title></head><body><center><h1>It works!</h1></center></body></html>\r\n\r\n";
        }else{
            // Bad Request
            response << "HTTP/1.1 400 Bad Request\r\nServer: nginx\r\nContent-Type: text/html\r\nLocation: https://errno104.com/\r\n\r\n<html>\r\n<head><title>Bad Request</title></head><body><center><h1>Bad Request</h1></center></body></html>\r\n\r\n";
        }
        fake_header_respond = true;
    }

    string key = Encryption::sha_hash(password);
    found = handshake_str.str().find(key);
    if (found != string::npos){
        response << Encryption::sha_hash("TODO"); //TODO: the password
        int first_packet_size = handshake_str.str().length() - found - key.size();
        if (first_packet_size == 0){
            this->read_buffer.pop_front();
            delete data;
        }else{
            int start = data->size - first_packet_size;
            for (int i = 0; i < first_packet_size; i++){
                data->buffer[i] = data->buffer[start + i];
            }
            data->size = first_packet_size;
        }

        this->rf->hook_recv = nullptr;
        this->active = true;
    }

    struct Data_Package *r_data = new struct Data_Package;
    memcpy(r_data->buffer,response.str().c_str(),response.str().size());
    r_data->size = response.str().size();
    r_data->sent = 0;
    this->write_buffer.push_back(r_data);
    this->wf->start();
}

/////////////////////////////////////
Peer_A::Peer_A(ev::default_loop *loop, int socket_id) {
    this->loop = loop;
    this->socket_id = socket_id;
    this->active = false;
    this->fake_header_respond = false;

    this->dispatcher_id = ++unique_id;
    Peer_A::interface_list[this->dispatcher_id] = this;
    this->password = "password"; // TODO: need to load from some file
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