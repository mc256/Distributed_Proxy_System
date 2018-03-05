//
// Created by mc on 2/23/18.
//

#include "Client_B.hpp"


void Client_B::generate_fake_request() {
    static string http_request = "GET / HTTP/1.1\r\nHost:errno104.com\r\nConnection: keep-alive\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: en-CA,en;q=0.9,zh;q=0.8,zh-CN;q=0.7,zh-TW;q=0.6,ja;q=0.5,fr;q=0.4\r\n";
    static string cookie = "Cookie: ";

    //Construct key
    //string key = "Hello world" + to_string(current_time) + password;
    string key = Encryption::sha_hash(this->peer->password);
    string request = http_request + cookie + key;
    //DEBUG(cout << "fake http request:" << endl << request << endl;)

    //Prepare package
    struct Data_Package *data = new struct Data_Package;
    data->size = request.length();
    data->sent = 0;
    for (int i = 0; i < request.length(); i++) { data->buffer[i] = request.at(i); }
    this->write_buffer.push_back(data);
}


void Client_B::remove_package_with_dispatcher_id(int id) {
    // TODO
}


/////////////////////////////////////
// STEP 1
void Client_B::start() {
    DEBUG(cout << this->peer->address << ":" << this->peer->port << " connected! socket id" << this->peer->descriptor
               << endl;)

    // prepare fake http request
    this->generate_fake_request();

    // write
    this->wf = new W_Filter(loop, &this->write_buffer, this->peer->descriptor, &this->wf);
    this->wf->set_keep(false);

    // read
    this->rf = new R_Filter(loop, &this->read_buffer, this->peer->descriptor, &this->rf, 0);
    this->rf->hook_recv = [this](R_Filter *rf) {
        this->verify_peer();
    };

    // start everything
    this->rf->start();
    this->wf->start();

}

// STEP 2
void Client_B::verify_peer() {
    struct Data_Package * data = this->read_buffer.front();
    //DEBUG(cout << string((char *) data->buffer) << endl;)

    read_stream.write((char *) data->buffer, data->size);

    // Check if key exists
    string key = Encryption::sha_hash(Client_Core::password_confirm);
    size_t found = read_stream.str().find(key);
    if (found != string::npos) {

        // Setup hooks
        this->rf->hook_recv = [this](R_Filter * rf){
            this->split_package();
        };
        this->rf->hook_closed = [this](R_Filter * rf){
            this->shutdown();
        };
        this->wf->hook_closed = [this](W_Filter * wf){
            this->shutdown();
        };

        // Calculate offset
        int first_packet_size = read_stream.str().length() - found - key.size();

        // Reset Read Stream
        this->read_size = 0;
        this->package_offset = 0;
        this->read_stream.str("");
        this->read_stream.clear();

        // Cut useless package
        if (first_packet_size == 0){
            this->read_buffer.pop_front();
            delete data;
        }else{
            this->package_offset = data->size - first_packet_size;
            data->size = first_packet_size;
            this->split_package();
        }

        // Mark available
        this->active = true;
        Client_B::available_list.push_back(this);

    }else{
        this->read_buffer.pop_front();
        delete data;
    }

}

void Client_B::split_package(){
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

void Client_B::shutdown() {
    this->active = false;
    delete this;
}

/////////////////////////////////////
Client_B::Client_B(ev::default_loop *loop, struct Proxy_Peer *peer) {
    this->loop = loop;
    this->socket_id = peer->descriptor;
    this->active = false;

    this->peer = peer;

    this->dispatcher_id = ++unique_id;
    Client_B::interface_list[this->dispatcher_id] = this;

    this->read_stream.str("");
}

Client_B::~Client_B() {
    if (this->rf != nullptr){
        this->rf->passive_terminate();
    }
    if (this->wf != nullptr){
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

    Client_B::interface_list.erase(this->dispatcher_id);
    for (int i = 0; i < Client_B::available_list.size(); ++i) {
        if (Client_B::available_list[i] == this){
            Client_B::available_list.erase(Client_B::available_list.begin() + i);
            break;
        }
    }
}

string Client_B::info() {
    stringstream ss;
    ss << "Client_B -\t" << peer->address << ":" << peer->port;
    ss << "\t" << peer->descriptor << "\tR:" << read_buffer.size() << "\tW:" << write_buffer.size();
    ss << "\t" << (active ? "ON" : "OFF");
    return ss.str();
}


int Client_B::unique_id = 0;
vector<Client_B *> Client_B::available_list = vector<Client_B *>();
map<int, Client_B *> Client_B::interface_list = map<int, Client_B *>();
function<void (Client_B *, struct Data_Package *, Data_Meta *)> Client_B::hook_core_recv = nullptr;