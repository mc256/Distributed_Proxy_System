//
// Created by mc on 3/9/18.
//

#include "Peer_B.hpp"

void Peer_B::down_link_transmit() {

}

void Peer_B::send_signal(Packet *signal) {

}

void Peer_B::start() {
    if (socket_id <= 0){
        return;
    }

    // Read
    read_handler = new Async_Read(loop, socket_id, new char[MAX_BUFFER_SIZE], MAX_BUFFER_SIZE);
    read_handler->set_undefined_length(true);
    read_handler->read_event = [this](char *buf, size_t s) {
        read_buffer.push_back(new Packet(buf, s));
        down_link_transmit();

        read_handler->reset(new char[MAX_BUFFER_SIZE], MAX_BUFFER_SIZE);
        read_handler->start();
    };
    read_handler->failed_event = read_handler->closed_event = [this](char *buf, size_t s) {
        delete buf;
        down_link_transmit();
        send_signal(Packet::generate_closed_signal(interface_id));
    };

    // Write
    write_handler = new Async_Write(loop, socket_id);
    write_handler->wrote_event = [this](char *buf, size_t s) {
        delete write_pointer;
        write_pointer = nullptr;
        start_writer();
    };
    write_handler->closed_event = write_handler->failed_event = [this](char *buf, size_t s) {
        delete write_pointer;
        down_link_transmit();
        send_signal(Packet::generate_closed_signal(interface_id));
    };

    // Start
    read_handler->start();

}

void Peer_B::start_writer() {
    if (socket_id <= 0){
        // Connection is not ready yet
        return;
    }
    if (write_pointer != nullptr){
        // The Async Write is writing
        return;
    }
    if ((write_pointer = sort_buffer[sort_buffer_offset]) == nullptr){
        // The sort buffer is empty, no need to write
        // We may need to send notification to client TODO
        return;
    }

    // Remove from sort buffer
    sort_buffer.erase((int)sort_buffer_offset);

    // Send ACK. We may not need to send ACK all the time TODO
    send_signal(Packet::generate_ack_signal(interface_id, sort_buffer_offset));

    // Move to next position
    sort_buffer_offset ++;

    // Pass the packet to the writer
    write_handler->reset(write_pointer->buffer, write_pointer->length);
    write_handler->start();
}

void Peer_B::clear_read_buffer(size_t offset) {
    // up to and include the position offset
    while (read_buffer_offset <= offset) {
        read_buffer_offset++;
        delete read_buffer.front();
        read_buffer.pop_front();
    }
}

void Peer_B::terminate() {
    close(socket_id);
    delete this;
}

Peer_B::Peer_B(ev::default_loop *loop, int interface_id, Peer_Core *core) {
    // Copy Values
    this->loop = loop;
    this->core = core;
    this->socket_id = 0;
    this->interface_id = interface_id;

    // Initialize
    read_buffer_offset = 0;
    sort_buffer_offset = 0;
}

Peer_B::~Peer_B() {
    // Read
    delete read_handler;
    while (!read_buffer.empty()) {
        delete read_buffer.front();
        read_buffer.pop_front();
    }

    // Write
    delete write_handler;
    for (const auto &kv : sort_buffer) {
        delete kv.second;
    }

    // Parent
    core->connection_b.erase(interface_id);

}

string Peer_B::info() {
    stringstream ss;
    ss << "Peer_B   -\t socket ID:" << socket_id;
    ss << "\t\t";
    ss << "R:" << read_buffer.size();
    ss << " ";
    ss << "W:" << sort_buffer.size() << "(" << sort_buffer_offset << ")";
    return ss.str();
}