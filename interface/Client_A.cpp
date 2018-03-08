//
// Created by mc on 2/23/18.
//

#include "Client_A.hpp"

/////////////////////////////////////
Client_A::Client_A(ev::default_loop * loop, int socket_id, int interface_id, Client_Core * core) {
    // Copy Values
    this->loop = loop;
    this->core = core;
    this->socket_id = socket_id;
    this->interface_id = interface_id;

    // Read
    this->read_buffer_offset = 0;
    this->read_handler = new Async_Read(this->loop, socket_id, new char[MAX_BUFFER_SIZE], MAX_BUFFER_SIZE);
    this->read_handler->set_undefined_length(true);
    this->read_handler->read_event = [this](char * buf, size_t s){
        this->read_buffer.push_back(new Packet(buf, s));
        this->read_handler->reset(new char[MAX_BUFFER_SIZE], MAX_BUFFER_SIZE);
        this->read_handler->start();
    };
    this->read_handler->closed_event = [this](char * buf, size_t s){
        this->read_buffer.push_back(new Packet(buf, 0));
    };
    this->read_handler->failed_event = [this](char const * buf, size_t s){
        delete buf;
    };
    this->read_handler->start();

    //Write
    this->write_handler = new Async_Write(this->loop, socket_id);
    this->write_handler->wrote_event = [this](char * buf, size_t s){

    };
    this->write_handler->closed_event = [this](char * buf, size_t s){

    };
    this->write_handler->failed_event = [this](char * buf, size_t s){

    };
}

Client_A::~Client_A() {
    this->core->connection_a.erase(this->interface_id);
    delete this->read_handler;
    delete this->write_handler;
}

string Client_A::info() {
    stringstream ss;
    ss << "Client_A -\t socket ID:" << socket_id << "\t\tR:" << this->read_buffer.size() << " W:"
       << this->sort_buffer.size() << "(" << this->sort_buffer_offset << ")\t";
    return ss.str();
}