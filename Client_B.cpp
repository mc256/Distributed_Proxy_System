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
    DEBUG(cout << "fake http request:" << endl << request << endl;)

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

void Client_B::mark_unavailable(){
    this->active = false;
    vector<Client_B *>::iterator i = find(Client_B::available_list.begin(),Client_B::available_list.end(),this);
    if (i != Client_B::available_list.end()){
        Client_B::available_list.erase(i);
    }
    // need for clean up
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
        this->verify_peer(rf);
    };

    // start everything
    this->rf->start();
    this->wf->start();

}

// STEP 2
void Client_B::verify_peer(R_Filter *rf) {
    struct Data_Package * data = this->read_buffer.front();
    //DEBUG(cout << string((char *) data->buffer) << endl;)

    handshake_str << string((char *) data->buffer);
    string key = Encryption::sha_hash("TODO");
    size_t found = handshake_str.str().find(key);
    if (found == string::npos) {
        this->read_buffer.pop_front();
        delete data;
        return;
    }

    // Clean up handshake header
    size_t first_packet_size = handshake_str.str().length() - found - key.size();
    if (first_packet_size == 0){
        this->read_buffer.pop_front();
        delete data;
    }else{
        int start = this->read_buffer.front()->size - first_packet_size;
        for (int i = 0; i < first_packet_size; i++){
            this->read_buffer.front()->buffer[i] = this->read_buffer.front()->buffer[start + i];
        }
        this->read_buffer.front()->size = first_packet_size;
    }

    // mark as available
    this->rf->hook_recv = [this](R_Filter * rf){
        this->analyse_read_buffer();
    };
    this->rf->hook_closed = [this](R_Filter * rf){
        this->analyse_read_buffer();
        this->mark_unavailable();

    };
    this->wf->hook_closed = [this](W_Filter * wf){
        this->analyse_read_buffer();
        this->mark_unavailable();
    };


    this->active = true;
    Client_B::available_list.push_back(this);

}

void Client_B::analyse_read_buffer(){
    // split the packet

    struct Data_Package * d = new struct Data_Package;

    if (Client_B::hook_core_recv != nullptr){
        Client_B::hook_core_recv(this, d);
    }
}

/////////////////////////////////////
Client_B::Client_B(ev::default_loop *loop, struct Proxy_Peer *peer) {
    this->loop = loop;
    this->socket_id = peer->descriptor;
    this->active = false;

    this->peer = peer;

    this->dispatcher_id = ++unique_id;
    peer->interface = (void *) this;
    Client_B::interface_list[this->dispatcher_id] = this;

}

Client_B::~Client_B() {
    this->mark_unavailable();
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
function<void (Client_B *, struct Data_Package *)> Client_B::hook_core_recv = nullptr;