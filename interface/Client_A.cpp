//
// Created by mc on 2/23/18.
//

#include "Client_A.hpp"

void Client_A::up_link_transmit() {
    // Select available interface B
    size_t n = core->connection_b_available.size();
    if (n == 0) {
        return;
    }
    auto *b = core->connection_b_available[rand() % n];

    // Generate send queue
    for (size_t i = 0; i < read_buffer.size(); ++i) {
        if (read_buffer[i]->should_resend()) {
            read_buffer[i]->touch();
            b->write_buffer.push_back(
                    read_buffer[i]->generate_meta_packet(read_buffer_offset, i, interface_id)
            );
            b->write_buffer.push_back(read_buffer[i]->copy());
        }
    }
    b->start_writer();
}

void Client_A::send_signal(Packet *signal) {
    // Select available interface B
    size_t n = core->connection_b_available.size();
    if (n == 0) {
        return;
    }
    auto *b = core->connection_b_available[rand() % n];
    b->write_buffer.push_back(signal);
    b->start_writer();

}

void Client_A::start() {
    // Read
    read_handler = new Async_Read(loop, socket_id, new char[MAX_BUFFER_SIZE], MAX_BUFFER_SIZE);
    read_handler->set_undefined_length(true);
    read_handler->read_event = [this](char *buf, size_t s) {
        DEBUG(cout << "["<< socket_id << "]\t"<< "|==> " << s << endl;)
        read_buffer.push_back(new Packet(buf, s));
        up_link_transmit();

        read_handler->reset(new char[MAX_BUFFER_SIZE], MAX_BUFFER_SIZE);
        read_handler->start();
    };
    read_handler->failed_event = read_handler->closed_event = [this](char *buf, size_t s) {
        DEBUG(cout << "["<< socket_id << "]\t"<< "|==x " << s << endl;)
        delete buf;
        up_link_transmit();
        send_signal(Packet::generate_closed_signal(interface_id));
        close(socket_id);
        delete this;
    };

    //Write
    write_handler = new Async_Write(loop, socket_id);
    write_handler->wrote_event = [this](char *buf, size_t s) {
        DEBUG(cout << "["<< socket_id << "]\t"<< "|<== " << s << endl;)
        delete write_pointer;
        write_pointer = nullptr;
        start_writer();
    };
    write_handler->closed_event = write_handler->failed_event = [this](char *buf, size_t s) {
        DEBUG(cout << "["<< socket_id << "]\t"<< "|x== " << s << endl;)
        delete write_pointer;
        up_link_transmit();
        send_signal(Packet::generate_closed_signal(interface_id));
        close(socket_id);
        delete this;
    };

    //Start
    read_handler->start();
}

void Client_A::start_writer() {
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
    if (Encryption::chance(ACK_CHANCE)) send_signal(Packet::generate_ack_signal(interface_id, sort_buffer_offset));

    // Move to next position
    sort_buffer_offset ++;

    // Pass the packet to the writer
    write_handler->reset(write_pointer->buffer, write_pointer->length);
    write_handler->start();
}

void Client_A::clear_read_buffer(size_t offset) {
    // up to and include the position offset
    while (read_buffer_offset <= offset) {
        read_buffer_offset++;
        delete read_buffer.front();
        read_buffer.pop_front();
    }
}

void Client_A::terminate() {
    close(socket_id);
    delete this;
}

Client_A::Client_A(ev::default_loop *loop, int socket_id, int interface_id, Client_Core *core) {
    // Copy Values
    this->loop = loop;
    this->core = core;
    this->socket_id = socket_id;
    this->interface_id = interface_id;

    // Initialize
    read_buffer_offset = 0;
    sort_buffer_offset = 0;
}

Client_A::~Client_A() {
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

    // Parents
    core->connection_a.erase(interface_id);
}

string Client_A::info() {
    stringstream ss;
    ss << "Client_A -\t socket ID:" << socket_id;
    ss << "\t\t";
    ss << "R:" << read_buffer.size();
    ss << " ";
    ss << "W:" << sort_buffer.size() << "(" << sort_buffer_offset << ")";
    return ss.str();
}