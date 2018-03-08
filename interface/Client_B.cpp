//
// Created by mc on 2/23/18.
//

#include "Client_B.hpp"


void Client_B::generate_fake_request() {
    static string http_request = "GET / HTTP/1.1\r\nHost:errno104.com\r\nConnection: keep-alive\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: en-CA,en;q=0.9,zh;q=0.8,zh-CN;q=0.7,zh-TW;q=0.6,ja;q=0.5,fr;q=0.4\r\n";
    static string cookie = "Cookie: ";

    //Construct key
    //string key = "Hello world" + to_string(current_time) + password;
    string key = Encryption::sha_hash("TODO");
    string request = http_request + cookie + key;
    //DEBUG(cout << "fake http request:" << endl << request << endl;)

    //Prepare package
    struct Data_Package *data = new struct Data_Package;
    data->size = request.length();
    data->sent = 0;
    for (int i = 0; i < request.length(); i++) { data->buffer[i] = request.at(i); }

}


/////////////////////////////////////
// STEP 1
void Client_B::start() {


}

// STEP 2
void Client_B::verify_peer() {

}

void Client_B::split_package(){

}

void Client_B::shutdown() {

}

/////////////////////////////////////
Client_B::Client_B(ev::default_loop * loop, Proxy_Peer * peer, int interface_id, Client_Core * core) {
    // Copy Values
    this->loop = loop;
    this->peer = peer;
    this->interface_id = interface_id;
    this->core = core;

}

Client_B::~Client_B() {
    delete this->read_handler;
    delete this->write_handler;
}

string Client_B::info() {
    stringstream ss;
    ss << "Client_B -\t" << this->socket_id << "\t" << this->interface_id << endl;
    return ss.str();
}

