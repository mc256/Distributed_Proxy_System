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
    struct Data_Package *data = this->read_buffer.front();
    //DEBUG(cout << string((char *) data->buffer) << endl;)

    read_stream.write((char *) data->buffer, data->size);
    size_t found = read_stream.str().find('\n');
    if (found == string::npos) {
        return;
    }

    // Fake response;
    stringstream response;
    if (!fake_header_respond) {
        string method, uri, protocol;
        read_stream >> method >> uri >> protocol;
        if (protocol == "HTTP/1.1" && method == "GET") {
            response
                    << "HTTP/1.1 200 OK\r\nServer: nginx\r\nContent-Type: text/html\r\nLocation: https://errno104.com/\r\n\r\n<html>\r\n<head><title>Hello World!</title></head><body><center><h1>It works!</h1></center></body></html>\r\n\r\n";
        } else {
            // Bad Request
            response
                    << "HTTP/1.1 400 Bad Request\r\nServer: nginx\r\nContent-Type: text/html\r\nLocation: https://errno104.com/\r\n\r\n<html>\r\n<head><title>Bad Request</title></head><body><center><h1>Bad Request</h1></center></body></html>\r\n\r\n";
        }
        fake_header_respond = true;
    }

    // Check if key exists
    string key = Encryption::sha_hash(password);
    found = read_stream.str().find(key);
    if (found != string::npos) {
        response << Encryption::sha_hash("TODO"); //TODO: the password

        // Setup hook
        this->rf->hook_recv = [this](R_Filter *rf) {
            this->split_package();
        };
        this->rf->hook_closed = [this](R_Filter * rf){
            this->shutdown();
        };

        // Cut useless package
        int first_packet_size = read_stream.str().length() - found - key.size();
        cout << read_stream.str() << endl;

        // Reset Read Stream
        this->read_size = 0;
        this->package_offset = 0;
        this->read_stream.str("");
        this->read_stream.clear();

        if (first_packet_size == 0) {
            this->read_buffer.pop_front();
            delete data;
        } else {
            this->package_offset = data->size - first_packet_size;
            data->size = first_packet_size;
            this->split_package();
        }


        // Mark available
        this->active = true;
        Peer_A::available_list.push_back(this);

    } else {
        this->read_buffer.pop_front();
    }

    // Send back response
    auto *r_data = new struct Data_Package;
    memcpy(r_data->buffer, response.str().c_str(), response.str().size());
    r_data->size = response.str().size();
    r_data->sent = 0;
    this->write_buffer.push_back(r_data);
    this->wf->start();
}

void Peer_A::split_package() {
    while (!this->read_buffer.empty()) {
        auto *d = this->read_buffer.front();
        auto size = this->read_stream.str().size();

        if (size < sizeof(struct Data_Meta)) {
            auto diff_1 = sizeof(struct Data_Meta) - size;
            auto diff_2 = d->size - this->package_offset;
            if (diff_2 <= diff_1) {
                // Use the entire package for the meta information
                this->read_stream.write((char *) d->buffer + this->package_offset, diff_2);
                this->package_offset = 0;
                this->read_buffer.pop_front();
                delete d;
                continue;
            } else {
                // there are still some useful information in the package
                this->read_stream.write((char *) d->buffer + this->package_offset, diff_1);
                this->package_offset += diff_1;
                size += diff_1;
            }
        }

        if (size == sizeof(struct Data_Meta)) {
            this->package_holder = new struct Data_Package;
            this->meta_holder = new struct Data_Meta;
            memcpy(this->meta_holder, this->read_stream.str().c_str(), sizeof(struct Data_Meta));
            memcpy(&this->read_size, &this->meta_holder->size, sizeof(int));
            this->package_holder->sent = 0;
        }

        //this->package_holder->size + sizeof(struct Data_Meta) == size
        if (size >= sizeof(struct Data_Meta)) {
            auto diff_1 = this->read_size + sizeof(struct Data_Meta) - size;
            auto diff_2 = d->size - this->package_offset;
            if (diff_1 < diff_2){
                //copy
                this->read_stream.write((char *) d->buffer + this->package_offset, diff_1);
                //set offset
                this->package_offset += diff_1;
                //send signal to hook
            }else if (diff_1 == diff_2){
                //copy
                this->read_stream.write((char *) d->buffer + this->package_offset, diff_1);
                //reset offset to zero
                this->package_offset = 0;
                //delete package
                this->read_buffer.pop_front();
                delete d;
                //send signal to hook
            }else{
                //copy
                this->read_stream.write((char *) d->buffer + this->package_offset, diff_2);
                //reset offset to zero
                this->package_offset = 0;
                //delete package
                this->read_buffer.pop_front();
                delete d;
                continue;
            }

            memcpy(this->package_holder, this->read_stream.rdbuf() + sizeof(struct Data_Meta), this->read_size);
            if (this->hook_core_recv != nullptr) {
                this->hook_core_recv(this, this->package_holder, this->meta_holder);
            }
        }
    }
}

void Peer_A::shutdown() {
    this->active = false;
    delete this;
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
    for (int i = 0; i < Peer_A::available_list.size(); ++i) {
        if (Peer_A::available_list[i] == this){
            Peer_A::available_list.erase(Peer_A::available_list.begin() + i);
            break;
        }
    }
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
vector<Peer_A *> Peer_A::available_list = vector<Peer_A *>();
function<void(Peer_A *, struct Data_Package *, struct Data_Meta *)> Peer_A::hook_core_recv = nullptr;