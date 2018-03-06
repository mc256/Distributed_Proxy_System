//
// Created by mc on 3/6/18.
//

#include "Z_Test_Connect.hpp"

using namespace std;

void Z_Test_Connect::operator()(ev::io &watcher, int revents) {
    char buffer[1024];
    ssize_t t = recv(this->descriptor, buffer, 1024, 0);
    cout << "test" << endl;
    cout << string(buffer) << endl;
    cout << "testend" << endl;
    this->read_io_watcher.stop();
}

void Z_Test_Connect::write_cb(ev::io &watcher, int revents){
    cout << "sent test" << endl;
    string http_request = "GET / HTTP/1.1\r\nHost:errno104.com\r\nConnection: keep-alive\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n";
    ssize_t t = send(this->descriptor, http_request.c_str(), http_request.size(), 0);
    cout << t << "-" << http_request.size() << "sent testend" << endl;
    this->write_io_watcher.stop();
}


Z_Test_Connect::Z_Test_Connect(ev::default_loop * loop, string address, int port){

    //IP address
    struct in_addr ipv4;
    inet_pton(AF_INET, address.c_str(), &ipv4);

    //Socket address
    connect_address.sin_family = AF_INET;
    connect_address.sin_port = htons(port);
    connect_address.sin_addr = ipv4;
    connect_address_length = sizeof(struct sockaddr_in);

    //Create Socket
    CHECK(this->descriptor = socket(AF_INET, SOCK_STREAM, 0), <0, "Cannot create Socket.",);

    //Set unblock
    fcntl(this->descriptor, F_SETFL, fcntl(this->descriptor, F_GETFL) | O_NONBLOCK);

    //Set watcher
    this->read_io_watcher.set(*loop);
    this->read_io_watcher.set(this);
    this->read_io_watcher.set(this->descriptor, ev::READ);
    this->read_io_watcher.start();


    this->write_io_watcher.set(*loop);
    this->write_io_watcher.set<Z_Test_Connect, &Z_Test_Connect::write_cb>(this);
    this->write_io_watcher.set(this->descriptor, ev::WRITE);
    this->write_io_watcher.start();

    //Connect
    connect(this->descriptor, (sockaddr *) &this->connect_address, this->connect_address_length);
    cout << "connect" << endl;
}