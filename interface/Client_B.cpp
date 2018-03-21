//
// Created by mc on 2/23/18.
//

#include "Client_B.hpp"


tuple<char *, size_t > Client_B::generate_fake_request() {
    string fake_header = "GET / HTTP/1.1\r\nHost:errno104.com\r\nConnection: keep-alive\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: en-CA,en;q=0.9,zh;q=0.8,zh-CN;q=0.7,zh-TW;q=0.6,ja;q=0.5,fr;q=0.4\r\n";

    //Construct key
    string key = Encryption::sha_hash(peer->password);
    string request = fake_header + "Cookie: __utmc=" + key + "\r\n\r\n";

    return tuple<char *, size_t >(strdup(request.c_str()),request.length());
}


/////////////////////////////////////
// STEP 1
void Client_B::start() {
    // Read
    read_handler = new Async_Read(loop, socket_id, new char[MAX_BUFFER_SIZE], MAX_BUFFER_SIZE);
    read_handler->set_timeout(DEFAULT_TIMEOUT);
    read_handler->set_use_recv(true);
    read_handler->recv_event = [this](char *buf, ssize_t s) {
        verify_peer(buf, s);
    };
    read_handler->closed_event =
    read_handler->failed_event = [this](char *buf, ssize_t s) {
        delete buf;
        delete this;
    };

    // Write
    char * request;
    size_t request_size;
    tie(request, request_size) = generate_fake_request();
    write_handler = new Async_Write(loop, socket_id, request, request_size);
    write_handler->set_timeout(DEFAULT_TIMEOUT);
    write_handler->wrote_event = [this](char *buf, ssize_t s) {
        delete buf;
        read_handler->start();
    };
    write_handler->closed_event =
    write_handler->failed_event = [this](char *buf, ssize_t s) {
        delete buf;
        delete this;
    };

    // Start
    write_handler->start();

}

// STEP 2
void Client_B::verify_peer(char *buf, ssize_t s) {
    if (s == MAX_BUFFER_SIZE){
        delete buf;
        delete this;
        return;
    }

    string content(buf, s);
    size_t found = content.find(key);

    if (found != string::npos) {
        delete buf;
        read_handler->stop_watchers();
        read_handler->set_timeout(0);
        read_handler->set_use_recv(false);
        prepare_for_use();
    }
}

// Features
void Client_B::prepare_for_use() {

    // Reading Configuration
    on_reading_data = false;
    read_handler->set_timeout(Encryption::get_random(DEFAULT_TIMEOUT, 10));
    read_handler->reset((char *) new Packet_Meta, sizeof(Packet_Meta));
    read_handler->read_event = [this](char *buf, ssize_t s) {
        DEBUG(cout << "["<< socket_id << "]\t"<<"<==| " << s << endl;)
        if (on_reading_data) {
            // Current Packet is DATA Packet
            auto *connection = core->connection_a[read_meta->dispatcher];
            if (connection == nullptr) {
                // if the connection is closed, send back data
                write_buffer.push_back(Packet::generate_closed_signal(read_meta->dispatcher));
                start_writer();
                delete buf;
            } else {
                // if the connection is not close, inform the interface A
                connection->sort_buffer[read_meta->sequence] = new Packet(buf, (size_t) s);
                connection->start_writer();
            }

            // Prepare for the next packet
            delete read_meta;
            read_meta = nullptr;
            on_reading_data = false;
            read_handler->reset((char *) new Packet_Meta, sizeof(Packet_Meta));

        } else {
            // Current Packet is META Packet
            read_meta = (Packet_Meta *) buf;

            if (read_meta->signal == 0) {
                // Normal data
                on_reading_data = true;
                read_handler->reset(new char[read_meta->size], read_meta->size);

            } else if (read_meta->signal == 1) {
                // Close connection
                auto *connection = core->connection_a[read_meta->dispatcher];
                if (connection != nullptr) {
                    connection->terminate();
                }
                read_handler->reset();

            } else if (read_meta->signal == 2) {
                // ACK
                auto *connection = core->connection_a[read_meta->dispatcher];
                if (connection != nullptr) {
                    connection->clear_read_buffer(read_meta->sequence);
                }
                read_handler->reset();
            }
        }
        read_handler->start();
    };
    read_handler->closed_event = read_handler->failed_event = [this](char *buf, ssize_t s) {
        DEBUG(cout << "["<< socket_id << "]\t"<<"<==x " << s << endl;)
        close(socket_id);
        delete this;
    };


    // Writing Configuration
    write_handler->set_timeout(Encryption::get_random(DEFAULT_TIMEOUT, 10));
    write_handler->wrote_event = [this](char *buf, ssize_t s) {
        DEBUG(cout << "["<< socket_id << "]\t"<<"==>| " << s << endl;)
        if (s == 3){
            DEBUG(cout << "debug" << endl;)
        }
        delete write_pointer;
        write_pointer = nullptr;

        on_writing = false;
        start_writer();
    };
    write_handler->closed_event = write_handler->failed_event = [this](char *buf, ssize_t s) {
        DEBUG(cout << "["<< socket_id << "]\t"<<"==>x " << s << endl;)
        close(socket_id);
        delete this;
    };

    // Set Available
    core->connection_b_available.push_back(this);
    read_handler->start();
}

void Client_B::start_writer() {
    // This function can only be called after it is available
    if ((!on_writing) && (!write_buffer.empty())) {
        on_writing = true;

        write_pointer = write_buffer.front();
        write_buffer.pop_front();

        write_handler->reset(write_pointer->buffer, write_pointer->length);
        write_handler->start();
    }
}

/////////////////////////////////////
Client_B::Client_B(ev::default_loop *loop, Proxy_Peer *peer, int socket_id, Client_Core *core) {
    // Copy Values
    this->loop = loop;
    this->peer = peer;
    this->socket_id = socket_id;
    this->core = core;
    this->on_writing = false;
    this->on_reading_data = false;
    this->key = Encryption::sha_hash(core->password_confirm);
}

Client_B::~Client_B() {
    // Read
    delete read_handler;
    delete read_meta;

    // Write
    delete write_handler;
    while (!write_buffer.empty()) {
        auto *buf = write_buffer.front();
        write_buffer.pop_front();
        delete buf;
    }
    delete write_pointer;

    // Parent
    for (int i = 0; i < core->connection_b.size(); ++i) {
        if (core->connection_b[i] == this) {
            core->connection_b.erase(core->connection_b.begin() + i);
            break;
        }
    }
    for (int i = 0; i < core->connection_b_available.size(); ++i) {
        if (core->connection_b_available[i] == this) {
            core->connection_b_available.erase(core->connection_b_available.begin() + i);
            break;
        }
    }
}

string Client_B::info() {
    stringstream ss;
    ss << "Client_B -\t socket ID:" << socket_id;
    ss << "\t\t";
    ss << "W:" << write_buffer.size();
    return ss.str();
}

