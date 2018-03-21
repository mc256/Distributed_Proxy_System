//
// Created by mc on 3/20/18.
//

#include "File_Streamer.hpp"


void File_Streamer::start() {
    write_handler->start();
}


void File_Streamer::file_start() {
    send_buffer_size = file_stream.readsome(send_buffer, MAX_BUFFER_SIZE);

    write_handler->reset(send_buffer, send_buffer_size);
    write_handler->wrote_event = [this](char *buf, ssize_t s) {
        if (file_stream.good()) {
            send_buffer_size = file_stream.readsome(send_buffer, MAX_BUFFER_SIZE);
            if (send_buffer_size != 0) {
                write_handler->reset(send_buffer, send_buffer_size);
                write_handler->start();
                return;
            }
        }
        close(this->socket_id);
        delete this;

    };
    write_handler->start();
}


File_Streamer::File_Streamer(ev::default_loop *loop, int socket_id, string filename) {
    this->socket_id = socket_id;
    this->file_stream.open(filename, ifstream::in | ifstream::binary);

    char *buffer;
    size_t size;

    if (file_stream.good()) {
        string html_response = "HTTP/1.1 200 OK\r\nServer: nginx\r\nContent-Type: image/jpeg\r\nLocation: https://errno104.com/\r\n\r\n";

        buffer = strdup(html_response.c_str());
        size = html_response.length();
    } else {
        string html_response = "HTTP/1.1 400 Bad Request\r\nServer: nginx\r\nContent-Type: text/html\r\nLocation: https://errno104.com/\r\n\r\n<html>\r\n<head><title>Bad Request</title></head><body><center><h1>Bad Request</h1></center></body></html>\r\n\r\n";

        buffer = strdup(html_response.c_str());
        size = html_response.length();
    }


    write_handler = new Async_Write(loop, socket_id, buffer, size);
    write_handler->wrote_event = [this](char *buf, ssize_t s) {
        delete buf;
        if (file_stream.good()) {
            file_start();
        } else {
            close(this->socket_id);
        }
    };

    write_handler->closed_event =
    write_handler->failed_event = [this](char *buf, ssize_t s) {
        delete buf;
        close(this->socket_id);
        delete this;
    };
}

File_Streamer::~File_Streamer() {
    file_stream.close();
}