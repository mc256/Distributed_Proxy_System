//
// Created by mc on 3/9/18.
//

#include "Peer_A.hpp"


tuple<char *, size_t> Peer_A::generate_fake_http_response() {
    // Send confirm code and process to next step
    char *buffer;
    size_t buffer_size;

    stringstream response;
    response << "HTTP/1.1 200 OK\r\nServer: nginx\r\nContent-Type: image/jpeg\r\n";
    response << "Location: https://errno104.com/\r\n\r\n";
    response << (key = Encryption::sha_hash(key + core->confirm_password));

    buffer = strdup(response.str().c_str());
    buffer_size = response.str().length();
    return tuple<char *, size_t>(buffer, buffer_size);
}

void Peer_A::start() {
    // Read
    read_handler = new Async_Read(loop, socket_id, new char[MAX_BUFFER_SIZE], MAX_BUFFER_SIZE);
    read_handler->set_timeout(DEFAULT_TIMEOUT);
    read_handler->set_use_recv(true);
    read_handler->closed_event =
    read_handler->failed_event = [this](char *buf, ssize_t s) {
        delete buf;
        delete this;
    };
    read_handler->recv_event = [this](char *buf, ssize_t s) -> bool {
        if (buf[s - 4] == '\r' && buf[s - 3] == '\n' && buf[s - 2] == '\r' && buf[s - 1] == '\n') {
            read_handler->stop_watchers();

            string header(buf, s);
            delete buf;
            read_handler->set_use_recv(false);
            verify_client(header);
        } else if (s == MAX_BUFFER_SIZE) {
            string header(buf, s);
            delete buf;
            read_handler->set_use_recv(false);
            verify_client(header);
        }
    };

    // Write
    write_handler = new Async_Write(loop, socket_id);
    write_handler->set_timeout(DEFAULT_TIMEOUT);
    write_handler->wrote_event =
    write_handler->closed_event =
    write_handler->failed_event = [this](char *buf, ssize_t s) {
        delete buf;
        delete this;
    };

    // Start
    read_handler->start();
}

void Peer_A::verify_client(string s) {

    for (int i = -1; i < 2; i ++) {
        key = Encryption::sha_hash(peer->password + to_string(Encryption::get_current_time(TOTP_INTERVAL) + i));
        for (int j = 0; j < core->used_keys.size(); j ++){
            if (core->used_keys[j] == key){
                fail_verification();
                return;
            }
        }
        auto found = s.find(key);
        if (found != string::npos) {
            pass_verification();
            core->used_keys.push_back(key);
            if (core->used_keys.size() > 100) { core->used_keys.pop_front(); }
            return;
        }
    }
    fail_verification();
}

void Peer_A::fail_verification() {
    File_Streamer *fs = new File_Streamer(loop, socket_id, core->fake_source);
    fs->start();
    delete this;
}

void Peer_A::pass_verification() {
    char *response;
    size_t response_size;
    tie(response, response_size) = generate_fake_http_response();

    coder = new Encryption(Encryption::sha_hash(key + "DOWN"), Encryption::sha_hash(key + "UP"));

    write_handler->reset(response, response_size);
    write_handler->wrote_event = [this](char *buf, ssize_t s) {
        delete buf;
        prepare_for_use();
    };
    write_handler->start();
}

void Peer_A::prepare_for_use() {
    // Read
    on_reading_data = false;
    read_handler->set_timeout(Encryption::get_random(DEFAULT_TIMEOUT, 10));
    read_handler->reset((char *) new Packet_Meta, sizeof(Packet_Meta));
    read_handler->read_event = [this](char *buf, ssize_t s) {
        DEBUG(cout << "[" << socket_id << "]\t" << "|==> " << s << endl;)
        coder->decrypt(buf, s);

        if (on_reading_data) {
            // Current Packet is a DATA packet
            auto *connection = core->connection_b[read_meta->dispatcher];
            if (connection == nullptr) {
                if (read_meta->sequence == 0) {
                    // If sequence number is ZERO, then create a new connection
                    connection = core->connect_socks_server(read_meta->dispatcher);
                    connection->sort_buffer[read_meta->sequence] = new Packet(buf, (size_t) s);
                    connection->start_writer();
                } else {
                    // The connection is closed
                    write_buffer.push_back(Packet::generate_closed_signal(read_meta->dispatcher));
                    start_writer();
                    delete buf;
                }
            } else {
                // If the connection is open, inform the interface B
                connection->sort_buffer[read_meta->sequence] = new Packet(buf, (size_t) s);
                connection->start_writer();
            }

            // Prepare for the next
            delete read_meta;
            read_meta = nullptr;
            on_reading_data = false;
            read_handler->reset((char *) new Packet_Meta, sizeof(Packet_Meta));

        } else {
            // Current Packet is a META packet
            read_meta = (Packet_Meta *) buf;
            if (read_meta->signal == 0) {
                // Normal data
                on_reading_data = true;
                read_handler->reset(new char[read_meta->size], read_meta->size);
            } else if (read_meta->signal == 1) {
                // Close connection
                auto *connection = core->connection_b[read_meta->dispatcher];
                if (connection != nullptr) {
                    connection->terminate();
                }
                read_handler->reset();
            } else if (read_meta->signal == 2) {
                //ACK
                auto *connection = core->connection_b[read_meta->dispatcher];
                if (connection != nullptr) {
                    connection->clear_read_buffer(read_meta->sequence);
                }
                read_handler->reset();
            }
        }
        read_handler->start();
    };
    read_handler->closed_event = read_handler->failed_event = [this](char *buf, ssize_t s) {
        DEBUG(cout << "[" << socket_id << "]\t" << "|==x " << s << endl;)
        close(socket_id);
        delete this;
    };

    // Write
    write_handler->set_timeout(Encryption::get_random(DEFAULT_TIMEOUT, 10));
    write_handler->wrote_event = [this](char *buf, ssize_t s) {
        DEBUG(cout << "[" << socket_id << "]\t" << "|<== " << s << endl;)
        delete write_pointer;
        write_pointer = nullptr;

        on_writing = false;
        start_writer();
    };
    write_handler->closed_event = write_handler->failed_event = [this](char *buf, ssize_t s) {
        DEBUG(cout << "[" << socket_id << "]\t" << "|x== " << s << endl;)
        close(socket_id);
        delete this;
    };

    // Set Available
    core->connection_a_available.push_back(this);
    read_handler->start();
}

void Peer_A::start_writer() {
    if ((!on_writing) && (!write_buffer.empty())) {
        on_writing = true;

        write_pointer = write_buffer.front();
        write_buffer.pop_front();

        coder->encrypt(write_pointer->buffer, write_pointer->length);

        write_handler->reset(write_pointer->buffer, write_pointer->length);
        write_handler->start();
    }
}

Peer_A::Peer_A(ev::default_loop *loop, Proxy_Peer *peer, int descriptor, Peer_Core *core) {
    // Copy Values
    this->loop = loop;
    this->socket_id = descriptor;
    this->core = core;
    this->peer = peer;

    this->on_writing = false;
    this->on_reading_data = false;
    this->coder = nullptr;
}

Peer_A::~Peer_A() {
    delete coder;

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
    for (int i = 0; i < core->connection_a.size(); ++i) {
        if (core->connection_a[i] == this) {
            core->connection_a.erase(core->connection_a.begin() + i);
            break;
        }
    }
    for (int i = 0; i < core->connection_a_available.size(); ++i) {
        if (core->connection_a_available[i] == this) {
            core->connection_a_available.erase(core->connection_a_available.begin() + i);
            break;
        }
    }

}

string Peer_A::info() {
    stringstream ss;
    ss << "Peer_A   -\t socket ID:" << socket_id;
    ss << "\t\t";
    ss << "W:" << write_buffer.size();
    return ss.str();
}
